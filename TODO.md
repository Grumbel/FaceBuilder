# FaceBuilder — handoff

## Tip

- Bundle base: `e3f29e1`
- Current tip: cleanup of Ruby/Glade; Qt6 app is the only frontend

## Status

Feature parity with the old Ruby app is complete. Obsolete Ruby/GTK
sources were removed from the tip tree (still in git history).

## Optional later

- More face parts / sets (old content wishlist)
- Standardise part image origins
- Split hair front/back; clothes

## Build

```bash
cmake -B build && cmake --build build
FACEBUILDER_DATA=$PWD/data ./build/facebuilder
nix run
```
