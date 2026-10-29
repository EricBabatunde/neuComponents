import type { Sample } from "../lib/serial";
import { fit, grid, palette, type VisualizerFactory } from "./types";

// Hand-sweep profile: each reading becomes a dot, newest on the right.
// Sweeping the sensor across an edge shows up as a step between two distance levels.
const KEEP = 240;

export const tofSweep: VisualizerFactory = (canvas) => {
  const p = palette();
  let view = fit(canvas);
  new ResizeObserver(() => { view = fit(canvas); render(); }).observe(canvas);
  let trail: number[] = [];

  function render() {
    const { ctx, w, h } = view;
    const pad = { l: 56, r: 16, t: 16, b: 32 };
    const plotW = w - pad.l - pad.r, plotH = h - pad.t - pad.b;
    ctx.clearRect(0, 0, w, h);
    grid(ctx, w, h, p.line);

    // Y axis: 0 mm at top (sensor), far at bottom. Range grows in 500 mm steps.
    const far = Math.max(1000, Math.ceil(Math.max(0, ...trail) / 500) * 500);
    const y = (mm: number) => pad.t + (mm / far) * plotH;
    ctx.font = `500 12px ${p.mono}`;
    ctx.fillStyle = p.muted;
    ctx.textAlign = "right";
    ctx.textBaseline = "middle";
    for (let mm = 0; mm <= far; mm += 500) ctx.fillText(`${mm}`, pad.l - 8, y(mm));
    ctx.textAlign = "left";
    ctx.fillText("mm", 8, pad.t);
    ctx.textAlign = "right";
    ctx.fillText("time →", w - pad.r, pad.t + plotH + 16);

    ctx.strokeStyle = p.ink;
    ctx.lineWidth = 2;
    ctx.beginPath(); ctx.moveTo(pad.l, pad.t); ctx.lineTo(pad.l, pad.t + plotH); ctx.stroke();

    const step = plotW / KEEP;
    const x0 = pad.l + plotW - trail.length * step;
    trail.forEach((mm, i) => {
      const newest = i === trail.length - 1;
      ctx.fillStyle = newest ? p.red : p.ink;
      ctx.beginPath();
      ctx.arc(x0 + i * step, y(mm), newest ? 5 : 2, 0, Math.PI * 2);
      ctx.fill();
    });
  }

  return {
    draw(s: Sample) {
      // Status 0 means a valid range; other statuses are shown in readouts but not plotted.
      if (typeof s.d === "number" && (s.status ?? 0) === 0) {
        trail.push(s.d);
        if (trail.length > KEEP) trail.shift();
      }
      render();
    },
    clear() { trail = []; render(); },
    simulate(t) {
      // Sweep back and forth across a box edge: box top at 420 mm, floor at 880 mm.
      const onBox = Math.sin(t * 0.8) > 0.1;
      const d = (onBox ? 420 : 880) + (Math.random() - 0.5) * 12;
      return { d: Math.round(d), status: 0, signal: onBox ? 6.2 : 2.4, ambient: 0.3 };
    },
  };
};
