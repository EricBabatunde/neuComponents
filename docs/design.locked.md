# Design System: NeuComponents

```
Status: LOCKED
Project: NeuComponents
Locked on: 2026-10-29
```

> Every value below is final. Code references these tokens only. Preview: `docs/preview.html`.

---

## 0. Instructions for Claude

This file is the single source of truth. Every color, size, radius, shadow, duration, and easing in the code must reference a token defined here. If a new token is needed, stop, propose it to the user, and update this file before using it.

To add a new component page, follow `docs/AUTHORING.md`.

---

## 2. Anti-patterns (hard rules)

Avoid vibe-coded website patterns.

**Colors and visual**
- No default purple gradients unless the brand calls for them.
- No sparkles or emojis in hero headings.
- No generic glowing hover effects.

**Typography**
- Consistent weight hierarchy. No oversized headings paired with ultra-thin body text.
- Uniform line-height and paragraph spacing.
- Define a type scale and stick to it.

**Layout and components**
- Identical component placement across pages.
- Consistent border radius values (2 or 3 defined, plus full).
- Subtle hover states: 2 to 4px lift at most.
- Icons sized in proportion to adjacent text.
- No non-functional social icons.

**Animations and interactions**
- Use defined easing curves (cubic-bezier), never `linear` for UI motion (spinners excepted).
- Stagger timing on purpose, not by default.
- Every animation serves a purpose.

**UX behaviors**
- Loading states for all async actions.
- Progress indicators on buttons that trigger async work.
- Toggles, carousels, tabs, and accordions must actually work.
- Skeleton screens for data-heavy sections.

**Copywriting**
- Avoid em-dash overuse.
- No vague phrases such as "Launch faster", "Build your dreams", "Create without limits". Say what the thing does.
- No fake testimonials.
- No generic AI faces or placeholder names like "Sarah Chen".

**Core principle**
Create the design system first. Every spacing value, color, radius, weight, and animation references that system. Inconsistency signals vibe-coding more than any single element.

---

## 3. Project Brief

- **Project:** NeuComponents
- **Purpose:** Learning site for embedded hardware (sensors, actuators, modules)
- **Goal:** Pick any component, understand what it does, wire it, and see it running live.
- **Audience:** Mixed levels. Each page starts simple (what it is, how to wire it) and goes deeper below (how it measures, limits, pitfalls).
- **Page types:** Home (mini landing combined with a name-only catalog and search), Component page (one page per component), Guided tutorial
- **Color modes:** Light only
- **Framework:** Astro + TypeScript
- **Styling:** Plain CSS custom properties (`src/styles/tokens.css`)
- **Content source:** MDX files in `src/content/` with a strict frontmatter schema
- **Interactive widgets:** 2D pinout (hover a pin to see its function), wiring diagram in real-life wire colors with draw-in, live data visualizers (IMU airplane, ToF sweep profile)
- **External data:** Web Serial API (Chrome / Edge) reading JSON lines at 115200 baud from an ESP32-S3 running firmware in `firmware/<component>/`. Simulate mode feeds synthetic samples when no board is connected. Unsupported browsers see a clear message and can still use Simulate.
- **Deploy target:** Any static host
- **No code on the site.** Firmware lives in the repo; pages only say which folder to flash.

---

## 4. Color Palette

Sampled from the reference videos in `docs/vids/`.

| Token | Light | Dark | Use |
|-------|-------|------|-----|
| Primary | #DC211A | n/a | Accent: tags' square bullet, active outlines, highlighted words, focus ring |
| Primary Hover | #B91C14 | n/a | Hover and pressed state of primary; red text on off-white backgrounds |
| Secondary | #101114 | n/a | Label chips (white mono text on ink), primary buttons |
| Background | #F2F0EC | n/a | Warm off-white canvas with faint grid |
| Surface | #FFFFFF | n/a | Plates, cards, panels |
| Text Primary | #101114 | n/a | Headings, body copy, focus borders (2px) |
| Text Secondary | #6B6966 | n/a | Captions, helper text, placeholders |
| Border | #DCDCDC | n/a | Dividers, grid lines, quiet plates (decorative) |
| Focus Ring | #DC211A | n/a | Keyboard focus outline, 2px |
| Success | #187A42 | n/a | Learned / connected states |
| Warning | #B45309 | n/a | Caution callouts |
| Error | #DC211A | n/a | Errors (same as primary) |

**Contrast report:**
- Text #101114 on #FFFFFF 18.9:1, on #F2F0EC 16.6:1
- Text Secondary #6B6966 on #FFFFFF 5.5:1, on #F2F0EC 4.8:1
- Primary #DC211A on #FFFFFF 4.9:1 (any size); on #F2F0EC 4.3:1, so red text on the canvas only at 24px+ or use Primary Hover (6.5:1)
- #FFFFFF on Primary 4.9:1 (red chips)
- Success #187A42 on #FFFFFF about 5.3:1; Warning #B45309 on #FFFFFF 5.0:1
- Border #DCDCDC is decorative only. Interactive boundaries (inputs, buttons, focused cards) use Text Primary.

---

## 5. Tokens

### 5.1 Typography

- **Heading font:** Space Grotesk
- **Body font:** Space Grotesk
- **Mono font:** JetBrains Mono (tags, chips, specs, readouts, buttons)
- **Headings:** weight 700, tracking -0.02em
- **Body:** weight 400; emphasis 500
- **Size scale:** 12 / 14 / 16 / 20 / 24 / 32 / 40 / 48px
- **Line height:** headings 1.15, body 1.55
- **Mono labels:** 12px, weight 500, uppercase, tracking 0.14em, preceded by an 8px red square
- **Max line length:** 65ch for body copy

### 5.2 Spacing

4px base unit. Allowed values: 4, 8, 12, 16, 24, 32, 48, 64px. No other values.

**Density:** Comfortable. Plates pad 24px, rows 16px × 24px, sections gap 48px.

### 5.3 Border radius

- **Small** (chips): 2px
- **Medium** (buttons, inputs, callouts): 4px
- **Large** (cards, plates, panels): 6px
- **Full** (status dots, spinners): 9999px

### 5.4 Shadows

Borders do the elevation work. Shadows are reserved for floating layers (search results dropdown, tooltips).

- **Subtle:** `0 1px 2px rgba(0,0,0,0.05)`
- **Medium:** `0 4px 12px rgba(0,0,0,0.1)`
- **Strong:** `0 8px 24px rgba(0,0,0,0.15)`

### 5.5 Breakpoints and layout

- **Breakpoints:** sm 640px, md 768px, lg 1024px, xl 1280px
- **Content max width:** 1040px (doc pages: 240px sidebar + content + 200px TOC at lg+, up to 1280px)
- **Page gutter:** 16px mobile, 24px tablet, 32px desktop
- **Grid:** background grid 48px squares in Border color; content in CSS grid with 24px gaps, collapsing to one column below 640px

### 5.6 Motion

- **Intensity:** Subtle
- **Implementation:** CSS transitions and keyframes for UI; `<canvas>` with requestAnimationFrame for live visualizers
- **Durations:** fast 120ms (hover, press), base 200ms (fades, small moves), slow 320ms (wiring draw-in per wire, page sections)
- **Easing:**
  - Standard: cubic-bezier(0.2, 0, 0, 1)
  - Enter (decelerate): cubic-bezier(0, 0, 0.2, 1)
  - Exit (accelerate): cubic-bezier(0.4, 0, 1, 1)
- **Stagger step:** 40ms, capped at 8 items
- **Hover lift:** 2px
- **Animated elements and their purpose:**
  - Hover / focus on cards and rows: border turns red and card lifts 2px, showing the element is clickable.
  - Wiring draw-in: wires draw pin to pin in connection order (staggered), teaching which wire to place first.
  - Scroll reveals: page sections fade in and rise 8px once, marking a new step in the page's reading order. Content is visible at rest without JS.
  - Loading / connecting: spinner in the Connect button and a status chip, so the user knows the serial port is opening.
  - Catalog stagger: names enter in sequence on first load, guiding the eye down the list.
- **Reduced motion:** remove all transforms and movement; keep opacity changes up to 150ms. Live visualizers still follow real data (that motion is the content), but simulated data pauses on a still frame.

---

## 6. Components

### 6.1 Core patterns

**Buttons:** height 40px, padding 0 16px, radius 4px, JetBrains Mono 14px / 500 / uppercase / tracking 0.08em. Primary: ink fill, white text; hover red fill. Ghost: white fill, ink border and text; hover red border and text. Focus-visible: 2px red outline, 2px offset. Disabled: muted text, border color border, no hover. Loading: `aria-busy="true"`, white fill, spinner before label, label changes to the action in progress ("Connecting"), click ignored.

**Inputs:** height 48px, padding 0 16px, radius 4px, 2px ink border, white fill, 16px body font. Focus: 2px red outline, 2px offset. Error: red border plus message below in Primary Hover, 14px. Disabled: border color border, muted text.

**Cards:** white surface, 2px ink border, radius 6px, padding 24px, gap 12px. Clickable cards: hover border red and 2px lift. Quiet plates: 1px border color border instead of ink, used for secondary groups.

**Navigation:** top bar with two mono tags (left: "NeuComponents" home link with red square; right: page index like "02 · Component"). Doc pages add a left sidebar (component names, current one in ink with red square, others muted) and a right "On this page" TOC at lg+. Below 1024px the sidebar collapses into a "Components" disclosure button under the top bar; TOC hides. Prev / next links at the bottom of every component and tutorial page.

### 6.2 Purpose-specific components

- **Catalog list:** single bordered plate, one row per component name (20px / 500), right side shows status (Learned in success mono, or a line chip). Rows link to the component page. Search input above filters names instantly; empty result shows "No component named ..." with a clear-search button.
- **Component header:** card with mono tag ("Component · IMU"), H1 name, one-sentence summary, chips for interface, address, voltage, range. A "Mark as learned" toggle sits at the right.
- **Spec table:** two columns, mono labels muted, values in ink; rows separated by border color lines.
- **Pinout diagram:** SVG of the breakout board in its real PCB color with labelled pins. Hover or focus a pin to highlight it in red and show its function in a caption bar below.
- **Wiring diagram:** SVG of board-to-ESP32-S3 connections with real wire colors (red VCC, black GND, yellow SDA, green SCL, other signals as listed). Wires draw in on first view (320ms each, 40ms stagger). Below it, a numbered list of the same connections (the order is real: wire in that order).
- **Live panel:** bar with Connect / Disconnect button, status chip (Idle, Connecting, Streaming · N Hz, Error), and Simulate toggle; canvas on the left (4:3), mono readouts on the right (label 12px muted, value 32px). Stacks to one column below 640px.
- **Callouts:** note (ink left border) and warning (warning left border), 2px left border, radius 4px, padding 16px.
- **Tutorial steps:** numbered steps (the order matters), each a plate with step number in mono, title, body, optional embedded wiring or live panel.
- **Progress:** "Learned" state per component and "Done" per tutorial step, saved in localStorage; catalog rows show it.
- **TOC and prev / next:** generated from headings; prev / next follow catalog order.

### 6.3 Token export

```css
:root {
  --color-primary: #DC211A;
  --color-primary-hover: #B91C14;
  --color-secondary: #101114;
  --color-bg: #F2F0EC;
  --color-surface: #FFFFFF;
  --color-text: #101114;
  --color-text-muted: #6B6966;
  --color-border: #DCDCDC;
  --color-focus: #DC211A;
  --color-success: #187A42;
  --color-warning: #B45309;
  --color-error: #DC211A;

  --font-heading: "Space Grotesk", system-ui, sans-serif;
  --font-body: "Space Grotesk", system-ui, sans-serif;
  --font-mono: "JetBrains Mono", ui-monospace, monospace;

  --radius-sm: 2px;
  --radius-md: 4px;
  --radius-lg: 6px;
  --radius-full: 9999px;

  --shadow-subtle: 0 1px 2px rgba(0,0,0,0.05);
  --shadow-medium: 0 4px 12px rgba(0,0,0,0.1);
  --shadow-strong: 0 8px 24px rgba(0,0,0,0.15);

  --duration-fast: 120ms;
  --duration-base: 200ms;
  --duration-slow: 320ms;
  --ease-standard: cubic-bezier(0.2, 0, 0, 1);
  --ease-enter: cubic-bezier(0, 0, 0.2, 1);
  --ease-exit: cubic-bezier(0.4, 0, 1, 1);
}

/* Light only. No dark-mode block. */

@media (prefers-reduced-motion: reduce) {
  *, *::before, *::after {
    animation-duration: 0.01ms !important;
    transition-duration: 0.01ms !important;
    scroll-behavior: auto !important;
  }
}
```

---

## 7. Rules

1. Never introduce colors outside this palette. Wire and PCB colors in diagrams are the one exception: they show real-world colors and live only in component frontmatter.
2. Always use the spacing scale. No arbitrary values.
3. Keep border radius consistent per element type.
4. When in doubt, add more whitespace.
5. No raw values in component code. Reference tokens only.
6. Every animation has a purpose listed in Section 5.6.
7. Every async action has a loading state and an error state.
8. No placeholder people, fake testimonials, or stock AI faces.
9. Need a new token? Stop, propose it, update this file, then use it.

---

## 8. Decisions Log

- **Round 1, purpose:** Learning site; mixed audience; pages: home (mini landing + name-only catalog, no categories), component page as its own page, guided tutorial; features: nav + search + TOC, progress tracking, interactive 2D pinout / wiring in real-life colors, live sensor visualization. No code generation or code blocks on the site. Future pages are authored by Claude from `docs/AUTHORING.md`.
- **Round 2, color:** Palette sampled from frames of `docs/vids/*.mp4`: canvas #F2F0EC, white plates, ink #101114, red #DC211A. Hover red, muted text, success, and warning derived and accepted as proposed. Light only.
- **Round 3, type and shape:** Space Grotesk + JetBrains Mono; sharp radius 2 / 4 / 6; comfortable density.
- **Round 4, motion:** Subtle; CSS + canvas; hover / focus, wiring draw-in, scroll reveals, connecting states; 40ms stagger.
- **Round 5, stack:** Astro + TS; Web Serial without a backend; firmware per component in the repo (PlatformIO, ESP32-S3); first components MPU6050 and VL53L1X. VL53L1X uses a hand-sweep profile (single-zone sensor); ROI-scan mode noted as future work.
- **Preview v1 feedback:** Component work must be its own page, not a section of the home page. Applied in preview v2 and in the routing (`/components/<slug>/`). Everything else approved.
