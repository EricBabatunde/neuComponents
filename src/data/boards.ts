// Microcontroller boards that components wire to. Colors are real-world PCB colors,
// the one exception to the palette rule (see docs/design.locked.md §7).
export interface McuBoard {
  name: string;
  color: string;
  pins: Record<string, string>; // pin label -> what it is
}

export const mcu: McuBoard = {
  name: "ESP32-S3 DevKitC-1",
  color: "#1A1A1A",
  pins: {
    "3V3": "3.3 V output from the board's regulator",
    "5V": "5 V from USB",
    GND: "Ground",
    GPIO8: "Default I²C SDA in the Arduino core",
    GPIO9: "Default I²C SCL in the Arduino core",
    GPIO4: "General purpose I/O",
    GPIO5: "General purpose I/O",
  },
};
