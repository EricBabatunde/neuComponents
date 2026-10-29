// Per-browser progress. Storage can be missing or blocked, so every access is guarded.
const PREFIX = "nc:";

export function isDone(key: string): boolean {
  try {
    return localStorage.getItem(PREFIX + key) === "1";
  } catch {
    return false;
  }
}

export function setDone(key: string, done: boolean): void {
  try {
    if (done) localStorage.setItem(PREFIX + key, "1");
    else localStorage.removeItem(PREFIX + key);
  } catch {
    /* progress just won't persist */
  }
  document.dispatchEvent(new CustomEvent("nc:progress", { detail: { key, done } }));
}
