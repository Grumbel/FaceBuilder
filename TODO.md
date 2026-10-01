# FaceBuilder C++/Qt6 rewrite — TODO / handoff

## Base

- Effective base: `e3f29e1`
- Next bundle: `facebuilder-006.1-m5-polish-e3f29e1.bundle`

## Milestones

### M1–M4 ✅ (skeleton, model, interaction, persistence)
### M5 — Polish ✅
- [x] Examples path discovery (dev + installed/Nix share/facebuilder/examples)
- [x] Theme icons for New/Open/Save (with QStyle fallback)
- [x] HiDPI: PassThrough scale factor policy
- [x] View → Center Face (undoable; matches original center())

## Optional later
- [ ] Desktop entry / app icon
- [ ] Copy/paste face XML on clipboard
- [ ] More face parts / sets (old TODO list)

## Build / run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build
FACEBUILDER_DATA=$PWD/data ./build/facebuilder

nix build && nix run
```

Feature parity with the original Ruby app is essentially complete.
