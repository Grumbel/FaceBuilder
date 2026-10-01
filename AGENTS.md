# FaceBuilder — Agent Notes

## Project

FaceBuilder is a small face-composition tool. This tree is a **C++/Qt6 rewrite** of the original Ruby + GnomeCanvas application (see git history and the old `*.rb` / `facebuilder.glade` files).

Goal of the rewrite: keep the same feature set and data/XML format, modernise the implementation with Qt6 Widgets + QGraphicsView (the natural replacement for GnomeCanvas).

## Standing rules

- Author: Ingo Ruhnke <grumbel@gmail.com>
- Trailer on every commit: `Co-authored-by: Grok <grok@x.ai>`
- License: GPLv3+ (same as original). Use short SPDX headers.
- Deliverables: git bundles only (see session instructions). No parallel histories.
- Base commit for this work line: `3a83ba979d9a3e63ccfcd88a36b7a6074a00889d` (short: `3a83ba9`)
- Linux only for v1.
- Keep `data/` as-is (original PNGs). Do not reorganise assets yet.
- Keep the old XML format for load/save; no schema extensions for now.
- Feature parity first; no new features until the original behaviour works.

## Architecture (target)

```
src/
  main.cpp
  mainwindow.{h,cpp}     # menus, toolbar, status, layout
  facescene.{h,cpp}      # QGraphicsScene
  facepartitem.{h,cpp}   # QGraphicsPixmapItem + transforms / mirroring
  face.{h,cpp}           # model: parts map + undo
  facepart.{h,cpp}       # single part data
  partbrowser.{h,cpp}    # category combo + icon view
  xmlfaceformat.{h,cpp}  # load/save old XML
  ...
data/                    # original face-part PNGs (unchanged)
resources/icons/         # toolbar icons (copied from original + new)
```

## Current tip

See TODO.md. Rewrite M1–M5 + desktop/clipboard complete.
