# FaceBuilder C++/Qt6 rewrite — TODO / handoff

## Base

- Effective base: `e3f29e1`
- Tip: `facebuilder-007.1-desktop-clipboard-e3f29e1.bundle`

## Done

- M1–M5: skeleton, model, interaction, persistence, polish
- Desktop entry + app icon install
- Copy/Paste face XML on the clipboard
- About dialog shows logo.png

## Optional / future (old Ruby TODO)

- More face parts; male/female/comic sets
- Standardise part origins
- Split hair front/back; clothes

## Build / run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build
FACEBUILDER_DATA=$PWD/data ./build/facebuilder
nix build && nix run
```
