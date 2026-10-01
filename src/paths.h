// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef PATHS_H
#define PATHS_H

#include <QString>

/** Absolute path to the face-part data directory (…/data). */
QString findDataRoot();

/**
 * Absolute path to examples/ (pirate.xml etc.).
 * Tries sibling of data/, then share/facebuilder/examples next to the binary.
 */
QString findExamplesDir(const QString &dataRoot);

#endif
