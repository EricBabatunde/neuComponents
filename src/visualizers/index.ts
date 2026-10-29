import type { VisualizerFactory } from "./types";
import { imuPlane } from "./imu-plane";
import { tofSweep } from "./tof-sweep";

// Key used in a component's `visualizer` frontmatter field.
export const visualizers: Record<string, VisualizerFactory> = {
  "imu-plane": imuPlane,
  "tof-sweep": tofSweep,
};
