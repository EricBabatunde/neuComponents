import type { Sample } from "../lib/serial";

export interface Visualizer {
  draw(sample: Sample): void; // render one new sample
  simulate(seconds: number): Sample; // synthetic sample for Simulate mode
  clear(): void;
}

export type VisualizerFactory = (canvas: HTMLCanvasElement) => Visualizer;

// Canvas reads palette tokens from CSS so colors stay in one place.
export function palette() {
  const cs = getComputedStyle(document.documentElement);
  const v = (name: string) => cs.getPropertyValue(name).trim();
  return {
    ink: v("--color-text"),
    muted: v("--color-text-muted"),
    red: v("--color-primary"),
    line: v("--color-border"),
    surface: v("--color-surface"),
    mono: v("--font-mono"),
  };
}

// Scale the backing store for sharp lines on high-DPI screens.
export function fit(canvas: HTMLCanvasElement) {
  const ctx = canvas.getContext("2d")!;
  const dpr = window.devicePixelRatio || 1;
  const { width, height } = canvas.getBoundingClientRect();
  canvas.width = Math.round(width * dpr);
  canvas.height = Math.round(height * dpr);
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  return { ctx, w: width, h: height };
}

export function grid(ctx: CanvasRenderingContext2D, w: number, h: number, color: string, step = 32) {
  ctx.strokeStyle = color;
  ctx.lineWidth = 1;
  ctx.beginPath();
  for (let x = step; x < w; x += step) { ctx.moveTo(x + 0.5, 0); ctx.lineTo(x + 0.5, h); }
  for (let y = step; y < h; y += step) { ctx.moveTo(0, y + 0.5); ctx.lineTo(w, y + 0.5); }
  ctx.stroke();
}
