# FaceBuilder

FaceBuilder is a small toy application that lets you construct faces by
putting together eyes, nose, mouth, head, hair and other parts. You can
move, scale and rotate each part. Faces are saved as simple XML files;
you can also export PNG or SVG.

This is a **C++/Qt6** application (Linux). The original Ruby + GTK +
GnomeCanvas version lives in the git history if you need it.

## Features

- ~100 face parts under `data/` (eyes, hair, hats, …)
- Drag parts on the canvas; Shift+drag locks horizontal movement
- Scale / rotate via toolbar, mouse wheel, or keyboard
- Undo / redo
- Load and save the original FaceBuilder XML format
- Export PNG and SVG
- On-canvas previous/next controls (optional)
- Copy / paste face XML on the clipboard

## Keyboard

| Key | Action |
|-----|--------|
| PgUp / PgDown | Scale up / down |
| Home / End | Rotate |
| Arrow keys | Nudge position |
| A | Next image in current category |
| E / O | Previous / next category |
| C | Center current part horizontally |
| R | Reload part images from disk |
| Ctrl+Z / Ctrl+Shift+Z | Undo / redo |

## Building with Nix

```bash
nix build
nix run
# or
nix develop   # shell with cmake + Qt6
```

## Building with CMake

Requirements: CMake ≥ 3.16, Qt 6 (Widgets, Svg), C++17 compiler.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
FACEBUILDER_DATA=$PWD/data ./build/facebuilder
```

Open a face from the command line:

```bash
./build/facebuilder examples/pirate.xml
```

## Data paths

- Development: set `FACEBUILDER_DATA` to the `data/` directory, or run
  from a layout where `data/` sits next to the binary’s parent.
- Installed: `share/facebuilder/data` and `share/facebuilder/examples`.

## License

GPLv3+. See `COPYING` and `AUTHORS`.
