import { isDone, setDone } from "./progress";

// [data-progress="key"] buttons toggle a done flag; [data-progress-label="key"] shows it.
function paint() {
  document.querySelectorAll<HTMLButtonElement>("[data-progress]").forEach((btn) => {
    const done = isDone(btn.dataset.progress!);
    btn.setAttribute("aria-pressed", String(done));
    const label = btn.querySelector("[data-label]");
    if (label) label.textContent = done ? btn.dataset.on! : btn.dataset.off!;
  });
  document.querySelectorAll<HTMLElement>("[data-progress-label]").forEach((el) => {
    el.hidden = !isDone(el.dataset.progressLabel!);
  });
}

document.addEventListener("click", (e) => {
  const btn = (e.target as Element).closest<HTMLButtonElement>("[data-progress]");
  if (!btn) return;
  setDone(btn.dataset.progress!, btn.getAttribute("aria-pressed") !== "true");
});
document.addEventListener("nc:progress", paint);
paint();
