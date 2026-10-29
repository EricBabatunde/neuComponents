import { defineCollection } from "astro:content";
import { glob } from "astro/loaders";
import { z } from "astro/zod";

const hex = z.string().regex(/^#[0-9A-Fa-f]{6}$/, "Use a 6-digit hex color");

const components = defineCollection({
  loader: glob({ pattern: "**/*.mdx", base: "./src/content/components" }),
  schema: z.object({
    name: z.string(),
    kind: z.string(), // short type shown in tags, e.g. "IMU", "ToF distance"
    summary: z.string(), // one sentence, plain language
    order: z.number(), // catalog and prev / next order
    chips: z.array(z.string()).max(5),
    specs: z.array(z.object({ label: z.string(), value: z.string() })),
    board: z.object({
      color: hex, // real PCB color
      label: z.string(), // silkscreen text, e.g. "GY-521"
    }),
    pins: z.array(z.object({ label: z.string(), fn: z.string() })), // left to right as on the board
    wiring: z.array(
      z.object({
        pin: z.string(), // must match a pins[].label
        to: z.string(), // must match a pin in src/data/boards.ts
        wire: hex, // real wire color
        why: z.string(),
      }),
    ),
    firmware: z.string(), // folder under firmware/
    visualizer: z.string(), // key in src/visualizers/index.ts
    stream: z.array(
      z.object({
        key: z.string(), // JSON key sent by the firmware
        label: z.string(),
        unit: z.string().default(""),
        digits: z.number().default(1),
      }),
    ),
    pitfalls: z.array(z.object({ title: z.string(), body: z.string() })),
  }),
});

const tutorials = defineCollection({
  loader: glob({ pattern: "**/*.mdx", base: "./src/content/tutorials" }),
  schema: z.object({
    title: z.string(),
    summary: z.string(),
    order: z.number(),
    uses: z.array(z.string()), // component ids
    live: z.string().optional(), // component id whose live panel is embedded
    steps: z.array(z.object({ title: z.string(), body: z.string() })),
  }),
});

export const collections = { components, tutorials };
