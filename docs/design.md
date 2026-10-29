# Design System Template

```
Status: TEMPLATE
Project: {{PROJECT_NAME}}
Locked on: {{LOCK_DATE}}
```

> **Placeholder convention.** Every value to be filled looks like `{{TOKEN_NAME}}`.
> A locked file must contain zero `{{`. Run `grep -n '{{' design.locked.md` to check.

---

## 0. Instructions for Claude

Read this whole file before doing anything else.

**If `Status` is `TEMPLATE`:** do not write any site code, scaffolding, or styles. Run the intake in Section 1 first.

**If `Status` is `LOCKED`:** this file is the single source of truth. Every color, size, radius, shadow, duration, and easing in the code must reference a token defined here. If a new token is needed, stop, propose it to the user, and update this file before using it.

### Intake protocol

1. Run the five rounds in Section 1 **in order**, one round per turn, using the AskUserQuestion tool (max 4 questions per call).
2. Put the recommended option first in each question and mark it `(Recommended)`. Base recommendations on earlier answers (for example, the purpose chosen in Round 1 shapes the defaults in Rounds 3 to 5).
3. Hex values, font names, and project names come in through the free-text "Other" field. Accept any valid CSS color and normalize it to 6-digit hex.
4. After each round, restate the decisions in 2 to 4 short lines before asking the next round.
5. Values Claude derives (neutral ramps, hover shades, dark-mode variants, semantic colors, weights, layout widths, easing curves, component specs) are **proposals**. Show them in a table with contrast ratios and get confirmation. Never fill them in silently.
6. After Round 5, show a full summary of every token. On approval, build a **preview** before locking:
   - Write `preview.html` in the same folder: one self-contained static page, about 250 lines at most, no framework, no build step.
   - Show only what the user needs to judge the direction: color swatches with hex values, the type scale, the radius set, and 3 or 4 signature components from `{{PURPOSE_COMPONENTS}}` (for example a card, a label or chip, a button with its loading state, and one representative section).
   - Inline every token as a CSS custom property so a tweak is a one-line change.
   - Publish it as a private artifact and share the link. If artifacts are unavailable or the user prefers, render a PNG screenshot or leave the local file.
   - Ask for tweaks. Apply each one to the token values first, then patch and republish the preview. Repeat until the user approves.
   - The preview is not the build. Keep it small to save tokens; do not add pages, routing, or real content.
7. Once the preview is approved, lock the file:
   - Copy this file to `design.locked.md` in the same folder.
   - Fill every `{{...}}`, set `Status: LOCKED`, set `Locked on` to today's date.
   - Delete Section 1 (the question bank) from the locked copy and fill the Decisions Log in Section 8.
   - Run `grep -n '{{' design.locked.md` and confirm there are no matches.
8. Only then start building. Keep this template unchanged so it can be reused.

---

## 1. Intake Rounds

### Round 1: Purpose and audience

| # | Question | Options to offer |
|---|----------|------------------|
| 1.1 | What is the site's primary purpose? | Learning / documentation, Product marketing, Portfolio, Dashboard / web app, E-commerce, Other |
| 1.2 | Who is the audience and what is their skill level? | Beginners, Mixed, Practitioners, Experts |
| 1.3 | Which page types are needed? (multi-select) | Landing, Index / catalog, Detail page, Tutorial / guide, Search results, About, Contact |
| 1.4 | Project name (free text) and one sentence describing what a visitor should be able to do after using it | Free text |

Use the purpose to pre-select required components. Show the list and let the user add or remove items.

| Purpose | Required components |
|---------|---------------------|
| Learning / documentation | Sidebar navigation, search, breadcrumbs, table of contents, code blocks with copy button, callouts (note / tip / warning), prev / next links, progress indicator, diagrams with captions |
| Product marketing | Hero with one specific value statement, feature sections, real screenshots, pricing table, FAQ, single primary CTA per section |
| Portfolio | Project grid, project detail with media, about, contact form |
| Dashboard / web app | App shell, data tables, filters, empty states, skeleton loaders, toasts, form validation |
| E-commerce | Product grid, filters, product detail, cart drawer, checkout steps |

Fill `{{PURPOSE}}`, `{{AUDIENCE}}`, `{{PAGE_TYPES}}`, `{{PROJECT_NAME}}`, `{{SITE_GOAL}}`, `{{PURPOSE_COMPONENTS}}`.

### Round 2: Color

| # | Question | Options to offer |
|---|----------|------------------|
| 2.1 | Primary color (main actions, key UI). | Free-text hex, or 3 suggestions that fit the purpose |
| 2.2 | Secondary color (supporting elements, secondary buttons). | Free-text hex, or 3 suggestions derived from the primary |
| 2.3 | Color mode support. | Light only, Dark only, Both with system preference, Both with a toggle |
| 2.4 | Neutral temperature for backgrounds, text, borders. | Cool gray, Warm gray, Pure gray, Tinted toward primary |

Then Claude proposes Background, Surface, Text Primary, Text Secondary, Border, Primary Hover, Focus Ring, Success, Warning, Error, for each enabled mode. Report WCAG contrast for each text and UI pair:

- Body text on background and surface: at least **4.5:1**
- Large text (24px+, or 18.66px+ bold) and UI boundaries: at least **3:1**
- Primary color as button fill: label must meet 4.5:1

If the user's brand color fails, offer an adjusted shade for text use and keep the original for fills.

### Round 3: Typography and shape

| # | Question | Options to offer |
|---|----------|------------------|
| 3.1 | Font pairing (Google Fonts). | 3 pairings suited to the purpose, each as heading / body / mono. Example: Inter / Inter / JetBrains Mono |
| 3.2 | Type scale. | Default 12 / 14 / 16 / 20 / 24 / 32 / 40 / 48px, Compact (drop 48), Editorial (body 18px) |
| 3.3 | Corner personality. | Sharp 2 / 4 / 6px, Soft 4 / 8 / 12px, Round 8 / 12 / 20px |
| 3.4 | Density. | Compact, Comfortable, Spacious |

Density maps to component padding: Compact uses the lower spacing step, Spacious the higher one.

### Round 4: Motion

| # | Question | Options to offer |
|---|----------|------------------|
| 4.1 | Motion intensity. | None (state changes only), Subtle (fades, small lifts), Expressive (scroll-driven, choreographed sequences) |
| 4.2 | Implementation. | CSS only (transitions + keyframes), Motion (formerly Framer Motion, for React), GSAP (timelines, scroll triggers), View Transitions API |
| 4.3 | What animates? (multi-select) | Hover / press feedback, Page transitions, Scroll reveals, Diagrams and illustrations, Loading and progress, Expand / collapse |
| 4.4 | Stagger lists and grids on entry? | Yes (state step in ms), No |

For each element selected in 4.3, write one line in Section 5.6 stating **what it communicates**. If an animation has no stated purpose, cut it.

`prefers-reduced-motion: reduce` support is mandatory and not asked. Under reduced motion, keep opacity changes up to 150ms and remove all movement.

### Round 5: Tech stack and content

| # | Question | Options to offer |
|---|----------|------------------|
| 5.1 | Framework. | Astro (content-heavy, fast), Next.js (app-like, React), Vite + React (SPA), Plain HTML / CSS / JS |
| 5.2 | Styling approach. | CSS custom properties, Tailwind with tokens mapped from Section 6, CSS Modules |
| 5.3 | Content source. | Markdown / MDX files in repo, Headless CMS, JSON data files, Hardcoded |
| 5.4 | Special interactive widgets (free text). | Examples: interactive diagrams, live code editors, calculators, 3D viewers, simulators |

Also ask:
- **External data sources:** does the site read from APIs, devices (USB / serial / Bluetooth), or real-time streams? If yes, ask how the data arrives (direct browser API, local bridge, server) and what happens when the source is missing (offline message, simulated data).
- **Deploy target**, if it affects the build (static host, Vercel, Netlify, self-hosted).

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
- Use defined easing curves (cubic-bezier), never `linear` for UI motion.
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

- **Project:** {{PROJECT_NAME}}
- **Purpose:** {{PURPOSE}}
- **Goal:** {{SITE_GOAL}}
- **Audience:** {{AUDIENCE}}
- **Page types:** {{PAGE_TYPES}}
- **Color modes:** {{COLOR_MODES}}
- **Framework:** {{FRAMEWORK}}
- **Styling:** {{STYLING_APPROACH}}
- **Content source:** {{CONTENT_SOURCE}}
- **Interactive widgets:** {{INTERACTIVE_WIDGETS}}
- **External data:** {{EXTERNAL_DATA}}
- **Deploy target:** {{DEPLOY_TARGET}}

---

## 4. Color Palette

| Token | Light | Dark | Use |
|-------|-------|------|-----|
| Primary | {{COLOR_PRIMARY}} | {{COLOR_PRIMARY_DARK}} | Main actions, key UI elements |
| Primary Hover | {{COLOR_PRIMARY_HOVER}} | {{COLOR_PRIMARY_HOVER_DARK}} | Hover and pressed state of primary |
| Secondary | {{COLOR_SECONDARY}} | {{COLOR_SECONDARY_DARK}} | Supporting elements, secondary buttons |
| Background | {{COLOR_BG}} | {{COLOR_BG_DARK}} | Main canvas |
| Surface | {{COLOR_SURFACE}} | {{COLOR_SURFACE_DARK}} | Cards, modals, elevated elements |
| Text Primary | {{COLOR_TEXT}} | {{COLOR_TEXT_DARK}} | Headings, body copy |
| Text Secondary | {{COLOR_TEXT_MUTED}} | {{COLOR_TEXT_MUTED_DARK}} | Captions, helper text, placeholders |
| Border | {{COLOR_BORDER}} | {{COLOR_BORDER_DARK}} | Dividers, input outlines |
| Focus Ring | {{COLOR_FOCUS}} | {{COLOR_FOCUS_DARK}} | Keyboard focus outline |
| Success | {{COLOR_SUCCESS}} | {{COLOR_SUCCESS_DARK}} | Confirmations, valid states |
| Warning | {{COLOR_WARNING}} | {{COLOR_WARNING_DARK}} | Caution callouts |
| Error | {{COLOR_ERROR}} | {{COLOR_ERROR_DARK}} | Errors, destructive actions |

If only one mode is enabled, write `n/a` in the unused column.

**Contrast report:** {{CONTRAST_REPORT}}

---

## 5. Tokens

### 5.1 Typography

- **Heading font:** {{FONT_HEADING}}
- **Body font:** {{FONT_BODY}}
- **Mono font:** {{FONT_MONO}}
- **Headings:** weight {{WEIGHT_HEADING}}, tracking tight (-0.01em to -0.02em)
- **Body:** weight {{WEIGHT_BODY}}
- **Size scale:** {{TYPE_SCALE}}
- **Line height:** headings 1.1 to 1.25, body 1.5 to 1.6
- **Max line length:** 65 to 75 characters for body copy

### 5.2 Spacing

4px base unit. Allowed values: 4, 8, 12, 16, 24, 32, 48, 64px. No other values.

**Density:** {{DENSITY}}

### 5.3 Border radius

- **Small** (inputs, chips): {{RADIUS_SM}}
- **Medium** (cards, buttons): {{RADIUS_MD}}
- **Large** (modals, containers): {{RADIUS_LG}}
- **Full** (avatars, pills): 9999px

### 5.4 Shadows

- **Subtle:** `0 1px 2px rgba(0,0,0,0.05)`
- **Medium:** `0 4px 12px rgba(0,0,0,0.1)`
- **Strong:** `0 8px 24px rgba(0,0,0,0.15)`

In dark mode, shadows barely read. Show elevation with a lighter surface or a 1px border instead.

### 5.5 Breakpoints and layout

- **Breakpoints:** sm 640px, md 768px, lg 1024px, xl 1280px
- **Content max width:** {{CONTAINER_MAX}}
- **Page gutter:** 16px mobile, 24px tablet, 32px desktop
- **Grid:** {{GRID}}

### 5.6 Motion

- **Intensity:** {{MOTION_INTENSITY}}
- **Implementation:** {{MOTION_LIBRARY}}
- **Durations:** fast {{DURATION_FAST}} (hover, press), base {{DURATION_BASE}} (fades, small moves), slow {{DURATION_SLOW}} (panels, page transitions)
- **Easing:**
  - Standard: {{EASE_STANDARD}}
  - Enter (decelerate): {{EASE_ENTER}}
  - Exit (accelerate): {{EASE_EXIT}}
- **Stagger step:** {{STAGGER_STEP}}
- **Hover lift:** {{HOVER_LIFT}} (2 to 4px max)
- **Animated elements and their purpose:**
  {{MOTION_INVENTORY}}
- **Reduced motion:** remove all transforms and movement; keep opacity changes up to 150ms.

---

## 6. Components

### 6.1 Core patterns

**Buttons:** {{BUTTON_SPEC}}
Must include: height, horizontal padding, radius token, primary / secondary / ghost variants, hover, active, focus-visible, disabled, and a loading state that shows a spinner or progress and blocks double submits.

**Inputs:** {{INPUT_SPEC}}
Must include: height, padding, border, focus ring, error state with message, disabled state.

**Cards:** {{CARD_SPEC}}
Must include: padding, surface color, border or shadow (pick one per mode), hover behavior if clickable.

**Navigation:** {{NAV_SPEC}}
Must include: placement, active state, mobile behavior.

### 6.2 Purpose-specific components

{{PURPOSE_COMPONENTS}}

Each one gets a short spec: anatomy, states, which tokens it uses.

### 6.3 Token export

The locked file must include this block with values filled, so code maps 1:1 to tokens.

```css
:root {
  --color-primary: {{COLOR_PRIMARY}};
  --color-primary-hover: {{COLOR_PRIMARY_HOVER}};
  --color-secondary: {{COLOR_SECONDARY}};
  --color-bg: {{COLOR_BG}};
  --color-surface: {{COLOR_SURFACE}};
  --color-text: {{COLOR_TEXT}};
  --color-text-muted: {{COLOR_TEXT_MUTED}};
  --color-border: {{COLOR_BORDER}};
  --color-focus: {{COLOR_FOCUS}};
  --color-success: {{COLOR_SUCCESS}};
  --color-warning: {{COLOR_WARNING}};
  --color-error: {{COLOR_ERROR}};

  --font-heading: {{FONT_HEADING}};
  --font-body: {{FONT_BODY}};
  --font-mono: {{FONT_MONO}};

  --radius-sm: {{RADIUS_SM}};
  --radius-md: {{RADIUS_MD}};
  --radius-lg: {{RADIUS_LG}};
  --radius-full: 9999px;

  --shadow-subtle: 0 1px 2px rgba(0,0,0,0.05);
  --shadow-medium: 0 4px 12px rgba(0,0,0,0.1);
  --shadow-strong: 0 8px 24px rgba(0,0,0,0.15);

  --duration-fast: {{DURATION_FAST}};
  --duration-base: {{DURATION_BASE}};
  --duration-slow: {{DURATION_SLOW}};
  --ease-standard: {{EASE_STANDARD}};
  --ease-enter: {{EASE_ENTER}};
  --ease-exit: {{EASE_EXIT}};
}

/* Repeat color tokens with dark values here if dark mode is enabled */
{{DARK_MODE_BLOCK}}

@media (prefers-reduced-motion: reduce) {
  *, *::before, *::after {
    animation-duration: 0.01ms !important;
    transition-duration: 0.01ms !important;
    scroll-behavior: auto !important;
  }
}
```

If the styling approach is Tailwind, also map these variables into the Tailwind theme instead of using Tailwind's default palette.

---

## 7. Rules

1. Never introduce colors outside this palette.
2. Always use the spacing scale. No arbitrary values.
3. Keep border radius consistent per element type.
4. When in doubt, add more whitespace.
5. No raw values in component code. Reference tokens only.
6. Every animation has a purpose listed in Section 5.6.
7. Every async action has a loading state and an error state.
8. No placeholder people, fake testimonials, or stock AI faces.
9. Need a new token? Stop, propose it, update `design.locked.md`, then use it.

---

## 8. Decisions Log

Filled in the locked copy only. Record each intake answer and any proposal the user changed.

{{DECISIONS_LOG}}
