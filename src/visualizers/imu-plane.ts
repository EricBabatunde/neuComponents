import type { Sample } from "../lib/serial";
import { fit, grid, palette, type VisualizerFactory } from "./types";

// Wireframe airplane in body axes: x right wing, y up, z nose.
const POINTS: [number, number, number][] = [
  [0, 0, 1.6], [0, 0, -1.2], // nose, tail
  [-1.6, 0, -0.1], [1.6, 0, -0.1], // wing tips
  [-0.6, 0, -1.1], [0.6, 0, -1.1], // tailplane tips
  [0, 0.5, -1.1], // fin top
];
const EDGES: [number, number][] = [[0, 1], [2, 3], [2, 0], [3, 0], [2, 1], [3, 1], [4, 5], [1, 6]];

const rad = (d: number) => (d * Math.PI) / 180;

// Apply yaw (about y), pitch (about x), roll (about z).
function rotate([x, y, z]: [number, number, number], roll: number, pitch: number, yaw: number) {
  let c = Math.cos(rad(yaw)), s = Math.sin(rad(yaw));
  [x, z] = [x * c + z * s, -x * s + z * c];
  c = Math.cos(rad(pitch)); s = Math.sin(rad(pitch));
  [y, z] = [y * c + z * s, -y * s + z * c];
  c = Math.cos(rad(roll)); s = Math.sin(rad(roll));
  [x, y] = [x * c - y * s, x * s + y * c];
  return [x, y, z] as const;
}

export const imuPlane: VisualizerFactory = (canvas) => {
  const p = palette();
  let view = fit(canvas);
  let last: Sample = {};
  // Resizing the backing store clears the canvas, so redraw the last pose.
  new ResizeObserver(() => { view = fit(canvas); draw(last); }).observe(canvas);

  function draw(s: Sample) {
    last = s;
    const { ctx, w, h } = view;
    const roll = s.roll ?? 0, pitch = s.pitch ?? 0, yaw = s.yaw ?? 0;
    const k = Math.min(w, h) * 0.24, view3q = rad(20);
    ctx.clearRect(0, 0, w, h);
    grid(ctx, w, h, p.line);

    // Level reference: dashed horizon fixed to the world.
    ctx.setLineDash([6, 6]);
    ctx.strokeStyle = p.muted;
    ctx.beginPath(); ctx.moveTo(16, h / 2 + 0.5); ctx.lineTo(w - 16, h / 2 + 0.5); ctx.stroke();
    ctx.setLineDash([]);

    const pts = POINTS.map((v) => {
      const [x, y, z] = rotate(v, roll, pitch, yaw);
      // Camera sits slightly above and behind, looking along +z, so far points sit higher.
      return [w / 2 + x * k, h / 2 - (y * Math.cos(view3q) + z * Math.sin(view3q)) * k] as const;
    });
    ctx.lineWidth = 3;
    ctx.lineCap = "round";
    EDGES.forEach(([a, b], i) => {
      ctx.strokeStyle = i === 0 ? p.red : p.ink; // fuselage in red shows the nose direction
      ctx.beginPath(); ctx.moveTo(...pts[a]); ctx.lineTo(...pts[b]); ctx.stroke();
    });
    ctx.fillStyle = p.red;
    ctx.beginPath(); ctx.arc(pts[0][0], pts[0][1], 5, 0, Math.PI * 2); ctx.fill();
  }

  return {
    draw,
    clear: () => draw({}),
    simulate: (t) => ({
      roll: 30 * Math.sin(t * 0.9),
      pitch: 15 * Math.sin(t * 0.6 + 1),
      yaw: ((t * 20) % 360) - 180,
      ax: 0, ay: 0, az: 9.81,
      gx: 0, gy: 0, gz: 0.35,
      temp: 24.5,
    }),
  };
};
