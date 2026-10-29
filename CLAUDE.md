# NeuComponents

Astro + TypeScript site for learning embedded hardware by watching it work live. Sensors on an ESP32-S3 stream JSON lines over USB; pages read them with Web Serial and draw them on a canvas.

- **Design system:** `docs/design.locked.md` is the source of truth. Use tokens from `src/styles/tokens.css` only. No new colors, sizes, radii or durations without updating that file first.
- **Adding a component or tutorial:** follow `docs/AUTHORING.md`.
- **Design template for new projects:** `docs/design.md` (do not edit it for this project).

## Layout

- `src/content/components/*.mdx`: one file per component (schema in `src/content.config.ts`)
- `src/content/tutorials/*.mdx`: guided projects
- `src/pages/`: home + catalog, `components/[slug]`, `tutorials/[slug]`
- `src/components/`: Pinout, Wiring, LivePanel, ProgressToggle
- `src/lib/serial.ts`: Web Serial link; `src/lib/progress.ts`: localStorage progress
- `src/visualizers/`: canvas visualizers keyed by name
- `firmware/<id>/`: PlatformIO projects for the ESP32-S3

## Commands

- `npm run dev`: local server
- `npm run build`: static build to `dist/`
- `npm run check`: type check (needs TypeScript 6, pinned in devDependencies)
- `pio run -d firmware/<id>`: compile firmware; add `-t upload` to flash
