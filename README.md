# FaceBuilder

FaceBuilder is a small toy application that lets you construct faces by
putting together eyes, nose, mouth, head, hair and additional items.
You can move, scale and rotate each face-part. Results can be saved to
XML files. The repository currently ships ~100 face parts under `data/`.

This tree is a **C++/Qt6 rewrite** of the original Ruby + GnomeCanvas
program (still present in the history for reference).

## Building (Linux)

Requirements:

- CMake ≥ 3.16
- Qt 6 (Widgets, Svg)
- A C++17 compiler

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run (development):

```bash
# data/ is expected next to the source root
FACEBUILDER_DATA=$PWD/data ./build/facebuilder
```

Or from the build directory when the source tree layout is preserved:

```bash
./build/facebuilder
```

## Controls (planned parity with the original)

```
PgUp, PgDown: scale facepart
Home, End:    rotate facepart
Cursor keys:  move facepart
```

## License

GPLv3+ (see COPYING). Original author: Ingo Ruhnke <grumbel@gmail.com>.
