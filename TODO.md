# FaceBuilder C++/Qt6 rewrite — TODO / handoff

## Base

- Original tip / work-line base: `3a83ba979d9a3e63ccfcd88a36b7a6074a00889d` (`3a83ba9`)
- Current tip: (see git log)
- Next bundle: `facebuilder-002.1-model-browser-3a83ba9.bundle`

## Confirmed decisions (2026-10-01)

1. Qt6 Widgets + QGraphicsView
2. Keep old XML format (no extensions for now)
3. Keep `data/` inside the tree, unchanged
4. Linux only for v1
5. Feature parity with the original first; no new features yet

## Milestones

### M1 — Skeleton ✅
### M2 — Model + display parts ✅
- [x] Face / FacePart model
- [x] Load PNGs from `data/<category>/`
- [x] FacePartItem on the scene (incl. left/right mirroring)
- [x] Part browser: category combo + thumbnail grid + “none” (red X)

### M3 — Interaction (next)
- [ ] Select part on canvas, drag to move
- [ ] Scale / rotate via toolbar, keys, wheel
- [ ] Center H/V, reset
- [ ] QUndoStack for undo/redo
- [ ] Wire toolbar buttons to the current part

### M4 — Persistence
- [ ] XML load/save (old format, examples/*.xml)
- [ ] PNG export
- [ ] SVG export

### M5 — Polish
- [ ] Keyboard shortcuts match original
- [ ] Real toolbar icons
- [ ] HiDPI / packaging notes

## Build / run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
FACEBUILDER_DATA=$PWD/data ./build/facebuilder
```

Pick a category on the right, click a thumbnail — the part appears on the canvas.
Click the red X to clear that slot. File → New clears everything.

## Notes for next agent

- Work tree from the tip bundle; base remains `3a83ba9`.
- Old Ruby sources still present for reference.
- Mirrored types: eye, ear, eyebrow, mouthfold (FacePartItem draws a second flipped item).
- Transform order matches original Art::Affine chain.
