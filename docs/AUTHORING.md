# Adding a Component Page

Instructions for Claude (or anyone) adding a new component to NeuComponents. Read `docs/design.locked.md` first; every visual rule there applies.

A component needs three things:

1. A content file: `src/content/components/<id>.mdx`
2. Firmware: `firmware/<id>/` (PlatformIO project that prints JSON lines)
3. A visualizer: an existing key in `src/visualizers/index.ts`, or a new one

`mpu6050` and `vl53l1x` are the reference examples. Copy the closer one.

---

## 1. Content file

The id is the filename (lowercase, no spaces), and the page appears at `/components/<id>/`. The schema in `src/content.config.ts` rejects missing or misspelled fields at build time.

| Field | What to write |
|-------|---------------|
| `name` | Part number as printed, e.g. `BME280` |
| `kind` | 1 to 3 words, e.g. `Environment`, `Servo`, `ToF distance` |
| `summary` | One plain sentence: what it measures or does, and why someone would use it |
| `order` | Position in the sidebar and prev / next. Next free integer |
| `chips` | Up to 5 short facts: interface and address, supply, range, rate |
| `specs` | Label / value rows. Real datasheet numbers only. If unsure, say less, never guess |
| `board.color` | The real PCB color of the most common breakout, as hex |
| `board.label` | Silkscreen name, e.g. `GY-BME280` |
| `pins` | Every header pin, **left to right as printed on the board**, with a one-line function. Say "Leave unconnected" or "Not used here" where true |
| `wiring` | Only the pins that get connected, **in the order the user should wire them** (power, ground, then signals). `to` must be a key in `src/data/boards.ts` |
| `wiring[].wire` | Real wire colors: VCC `#D7261E`, GND `#1A1A1A`, SDA `#1E6FD9`, SCL `#F2C200`. Other signals: green `#2E9E44`, orange `#E8731A`, white `#F5F5F5`, purple `#7A3FB8` |
| `firmware` | Folder name under `firmware/` |
| `visualizer` | Key from `src/visualizers/index.ts` |
| `stream` | Readouts shown beside the canvas, max 6. `key` must match a JSON key the firmware prints. `digits` defaults to 1 |
| `pitfalls` | 2 to 4 real problems beginners hit, each with a title and a fix |

**Body (MDX below the frontmatter):** 2 to 4 `##` sections explaining how the part works, written for the mixed audience: first sentence plain, then the detail. Each `##` heading appears in the page's table of contents.

### Copy rules

- No code on the site. Refer to the firmware folder by name.
- Plain, specific language. No "unlock", "seamless", "powerful".
- Avoid em-dashes. Use a period or a colon.
- Numbers come with units. Use `°`, `m/s²`, `mm`, `I²C`.
- Never invent specs. If a value varies by breakout, say so.

---

## 2. Firmware

`firmware/<id>/platformio.ini` targets `esp32-s3-devkitc-1` with the Arduino framework (copy an existing one, change `lib_deps`). `src/main.cpp`:

- `Serial.begin(115200)`, `Wire.begin()` (SDA GPIO8, SCL GPIO9).
- On sensor-not-found, print a plain-text message and retry every second. Text lines are ignored by the site.
- Print **one JSON object per line**, numbers only, flat keys: `{"key":1.23,"other":4}`. Keep keys short and stable; they are the contract with `stream[].key` and the visualizer.
- Aim for 20 to 50 Hz.
- Comment the wiring at the top of the file.

Compile with `pio run -d firmware/<id>`.

---

## 3. Visualizer

Reuse one if it fits:

| Key | Shows | Reads |
|-----|-------|-------|
| `imu-plane` | Wireframe plane rotated by orientation | `roll`, `pitch`, `yaw` (degrees) |
| `tof-sweep` | Scrolling distance profile | `d` (mm), `status` (0 = valid) |

A new visualizer is a file in `src/visualizers/` exporting a `VisualizerFactory` (see `types.ts`):

- `draw(sample)` renders one sample on the canvas. Use `palette()` for colors and `grid()` for the background. Ink for data, red for the newest or most important mark. No other colors.
- `simulate(seconds)` returns a realistic fake sample so the page works without hardware. Model what the real sensor would show in a typical use.
- `clear()` resets and draws the empty state (axes, reference lines).
- Redraw on resize (`ResizeObserver` + `fit()`), since resizing clears the canvas.

Register it in `src/visualizers/index.ts`.

---

## 4. New MCU pins

If the wiring uses a pin not listed in `src/data/boards.ts`, add it there with a one-line description.

---

## 5. Checklist

- [ ] `npm run build` passes (schema validation included)
- [ ] `npm run check` shows 0 errors
- [ ] Pin order matches the physical board
- [ ] Wiring `pin` values exist in `pins`, `to` values exist in `boards.ts`
- [ ] Every `stream[].key` is printed by the firmware and by `simulate()`
- [ ] Simulate mode shows something meaningful
- [ ] `pio run -d firmware/<id>` compiles
- [ ] No new colors, sizes or radii outside `design.locked.md`

## Tutorials

`src/content/tutorials/<id>.mdx` with `title`, `summary`, `order`, `uses` (component ids), optional `live` (component id whose live panel is embedded), and `steps` (title + body, in the order the user does them). The MDX body is a short intro: what they will build and why it works.
