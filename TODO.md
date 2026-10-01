# FaceBuilder C++/Qt6 rewrite — TODO / handoff

## Base

- Effective base for bundles: `e3f29e1ab6fca67513e2a38686d08ee3eca4ae99` (`e3f29e1`)
- Current tip: (see git log)
- Next bundle: `facebuilder-004.1-m3-flake-e3f29e1.bundle`

## Confirmed decisions

1. Qt6 Widgets + QGraphicsView
2. Keep old XML format (no extensions for now)
3. Keep `data/` inside the tree, unchanged
4. Linux only for v1
5. Feature parity first

## Milestones

### M1 — Skeleton ✅
### M2 — Model + display parts ✅
### M3 — Interaction ✅
- [x] Select part on canvas, drag to move (Shift locks X)
- [x] Scale / rotate via toolbar, keys (PgUp/Down, Home/End), wheel
- [x] Center H/V, reset
- [x] QUndoStack for undo/redo
- [x] Toolbar buttons wired to current part
- [x] Selection outline on current part

### Nix ✅
- [x] `flake.nix` + `facebuilder.nix` (nixos-unstable, flake-utils)
- [x] `devShells.default`, `packages.default`
- [x] Install data + icons under share/facebuilder/

### M4 — Persistence (next)
- [ ] XML load/save (old format, examples/*.xml)
- [ ] PNG export
- [ ] SVG export

### M5 — Polish
- [ ] Open/Save file dialogs
- [ ] HiDPI / packaging notes

## Build / run

```bash
# CMake
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build
FACEBUILDER_DATA=$PWD/data ./build/facebuilder

# Nix
nix build
nix run
nix develop   # dev shell with Qt + cmake
```

## Notes

- Toolbar icons looked up relative to binary / data root / resources/.
- Undo: every part change (filename, offset, scale, rotation) is a QUndoCommand.
- Drag applies offset live; undo command pushed on mouse release.
