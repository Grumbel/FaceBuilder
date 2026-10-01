# FaceBuilder C++/Qt6 rewrite — TODO / handoff

## Base

- Original work-line start: `3a83ba9`
- Rebased onto upstream `e3f29e1` (Remove unused facebuilder-old.rb / clock.rb) on 2026-10-01
- Effective base for new bundles after rebase: `e3f29e1ab6fca67513e2a38686d08ee3eca4ae99` (`e3f29e1`)
- Current tip: (see git log)
- Next bundle: `facebuilder-003.1-rebased-e3f29e1.bundle`

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

## Notes for next agent

- Upstream moved past `3a83ba9`; rewrite commits were rebased onto `e3f29e1`.
- Apply the tip bundle on top of `e3f29e1` (current origin/master).
- Old Ruby sources still present for reference (clock.rb and facebuilder-old.rb were removed upstream).
- M3 was started in a prior session but not committed; implement from TODO.
