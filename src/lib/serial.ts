// Web Serial link to a board that prints one JSON object per line.
export type Sample = Record<string, number>;
export type LinkState = "unsupported" | "idle" | "connecting" | "streaming" | "error";

export class SerialLink {
  state: LinkState = SerialLink.supported() ? "idle" : "unsupported";
  error = "";
  private port?: SerialPort;
  private reader?: ReadableStreamDefaultReader<string>;
  private piped?: Promise<void>;

  constructor(
    private onSample: (s: Sample) => void,
    private onState: (s: LinkState) => void,
    private onText: (line: string) => void = () => {},
  ) {}

  static supported(): boolean {
    return typeof navigator !== "undefined" && "serial" in navigator;
  }

  private set(state: LinkState, error = "") {
    this.state = state;
    this.error = error;
    this.onState(state);
  }

  async connect(baudRate = 115200): Promise<void> {
    if (!SerialLink.supported()) return this.set("unsupported");
    this.set("connecting");
    try {
      this.port = await navigator.serial.requestPort();
      await this.port.open({ baudRate });
    } catch (e) {
      // Closing the port picker is not an error.
      if (e instanceof DOMException && e.name === "NotFoundError") return this.set("idle");
      return this.set("error", e instanceof Error ? e.message : String(e));
    }
    this.port.addEventListener("disconnect", () => this.set("error", "The board was unplugged."));
    this.set("streaming");
    void this.read();
  }

  private async read() {
    const decoder = new TextDecoderStream();
    this.piped = this.port!.readable!.pipeTo(decoder.writable as WritableStream<Uint8Array>).catch(() => {});
    this.reader = decoder.readable.getReader();
    let buffer = "";
    try {
      for (;;) {
        const { value, done } = await this.reader.read();
        if (done) break;
        buffer += value;
        const lines = buffer.split("\n");
        buffer = lines.pop() ?? "";
        for (const line of lines) this.parse(line.trim());
      }
    } catch (e) {
      if (this.state === "streaming") this.set("error", e instanceof Error ? e.message : String(e));
    }
  }

  private parse(line: string) {
    // Plain text (status and error messages from the firmware) goes to onText.
    if (!line.startsWith("{")) {
      if (line) this.onText(line);
      return;
    }
    try {
      const obj = JSON.parse(line);
      const sample: Sample = {};
      for (const [k, v] of Object.entries(obj)) if (typeof v === "number") sample[k] = v;
      this.onSample(sample);
    } catch {
      /* partial or garbled line */
    }
  }

  async disconnect(): Promise<void> {
    try {
      await this.reader?.cancel();
      await this.piped;
      await this.port?.close();
    } catch {
      /* port already gone */
    }
    this.reader = undefined;
    this.port = undefined;
    this.set(SerialLink.supported() ? "idle" : "unsupported");
  }
}
