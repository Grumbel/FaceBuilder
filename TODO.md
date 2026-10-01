# FaceBuilder C++/Qt6 rewrite — TODO / handoff

## Base

- Effective base: `e3f29e1` (`e3f29e1ab6fca67513e2a38686d08ee3eca4ae99`)
- Next bundle: `facebuilder-005.1-m4-persistence-e3f29e1.bundle`

## Milestones

### M1–M3 ✅
### M4 — Persistence ✅
- [x] XML load/save (original format)
- [x] Open / Save / Save As dialogs
- [x] PNG export (scene render 512×512)
- [x] SVG export (embedded base64 PNGs, mirrored parts)
- [x] Startup loads `examples/pirate.xml` when available

### M5 — Polish (next)
- [ ] Install examples path discovery when packaged via Nix
- [ ] Real stock toolbar icons for New/Open/Save
- [ ] HiDPI
- [ ] Optional: center-face action from original

## Build / run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build
FACEBUILDER_DATA=$PWD/data ./build/facebuilder

nix build && nix run
```

## Notes

- XML paths stored as `data/<type>/<file>.png`; resolved via data root.
- `XmlFaceFormat` + `FaceExport` are toolkit-thin and unit-testable.
- Export SVG transform chain mirrors the original Ruby exporter.
