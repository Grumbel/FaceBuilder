# FaceBuilder C++/Qt6 rewrite — TODO / handoff

## Base

- Original tip / work-line base: `3a83ba979d9a3e63ccfcd88a36b7a6074a00889d` (`3a83ba9`)
- Current tip: `0b11a45` (M1 skeleton)
- Next bundle: `facebuilder-001.1-qt6-skeleton-3a83ba9.bundle`

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

### M1 — Skeleton ✅
- [x] CMake + Qt6 project builds
- [x] MainWindow with menus + empty toolbar placeholders
- [x] QGraphicsView showing a white scene (512×512)
- [x] Basic New / Quit / About
- [x] Copy original toolbar icons into `resources/icons/`
- [x] AGENTS.md / TODO.md present
- [x] README updated for the rewrite

### M2 — Model + display parts (next)
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
- [ ] Wire real toolbar icons
- [ ] Status bar help text (done)
- [ ] About dialog (done)
- [ ] HiDPI
- [ ] Packaging notes (Flatpak later)

## Build / run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
FACEBUILDER_DATA=$PWD/data ./build/facebuilder
```

## Notes for next agent

- Work tree: `/tmp/facebuilder-cpp` or a clone from the tip bundle.
- Old Ruby sources are still present for reference; do not delete until M4 is solid.
- After the next tip, produce a cumulative bundle from base `3a83ba9`.
