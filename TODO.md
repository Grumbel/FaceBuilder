# FaceBuilder C++/Qt6 rewrite — TODO / handoff

## Base

- Original tip / work-line base: `3a83ba979d9a3e63ccfcd88a36b7a6074a00889d` (`3a83ba9`)
- Next bundle number: start at `facebuilder-001.…`

## Confirmed decisions (2026-10-01)

1. Qt6 Widgets + QGraphicsView
2. Keep old XML format (no extensions for now)
3. Keep `data/` inside the tree, unchanged
4. Linux only for v1
5. Feature parity with the original first; no new features yet

## UI reference

Original layout (from screenshot + glade):

- Menu bar: File, Edit, View, Help
- Toolbar: New, Open, Save, Save As, Undo, Redo, Copy, Paste,
  Scale−, Scale+, Center H, Center V, Rotate L, Rotate R, Reset properties
- Central: QGraphicsView (white 512×512-ish canvas) | Part browser (category combo + icon grid with “none” = red X)
- Status bar: keyboard help (PgUp/PgDown scale, Home/End rotate, cursors move, …)

## Milestones

### M1 — Skeleton (current focus)
- [ ] CMake + Qt6 project builds
- [ ] MainWindow with menus + empty toolbar placeholders
- [ ] QGraphicsView showing a white scene (512×512)
- [ ] Basic New / Quit
- [ ] Copy original toolbar icons into `resources/icons/`
- [ ] AGENTS.md / TODO.md present
- [ ] README updated for the rewrite

### M2 — Model + display parts
- [ ] Face / FacePart model
- [ ] Load PNGs from `data/<category>/`
- [ ] FacePartItem on the scene at default offsets
- [ ] Part browser lists categories + thumbnails (incl. “none”)

### M3 — Interaction
- [ ] Select part, drag to move
- [ ] Scale / rotate via toolbar, keys, wheel
- [ ] Center H/V, reset
- [ ] QUndoStack for undo/redo

### M4 — Persistence
- [ ] XML load/save (old format, examples/*.xml)
- [ ] PNG export
- [ ] SVG export

### M5 — Polish
- [ ] Keyboard shortcuts match original
- [ ] Status bar help text
- [ ] About dialog, icons, HiDPI
- [ ] Packaging notes (Flatpak later)

## Notes for next agent

- Work in `/tmp/facebuilder-cpp` (or a fresh clone from the tip bundle).
- After each meaningful tip, produce a cumulative git bundle under artifacts.
- Do not remove the old Ruby sources yet; keep them for reference until M4 is solid.
