// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef XMLFACEFORMAT_H
#define XMLFACEFORMAT_H

#include <QString>

class Face;

/**
 * Load/save the original FaceBuilder XML format:
 *
 *   <face>
 *     <head>
 *       <filename>data/head/0002.png</filename>
 *       <offset><x>0.0</x><y>-5.0</y></offset>
 *       <scale>1.0</scale>
 *       <rotation>0.0</rotation>
 *     </head>
 *     ...
 *   </face>
 *
 * Paths in the file are relative to the project root ("data/...").
 * dataRoot is the absolute path to the data/ directory.
 */
class XmlFaceFormat
{
public:
    explicit XmlFaceFormat(const QString &dataRoot);

    bool load(Face *face, const QString &filePath, QString *error = nullptr) const;
    bool save(const Face *face, const QString &filePath, QString *error = nullptr) const;

    /** Convert a stored "data/..." path to an absolute filesystem path. */
    QString resolveFilename(const QString &stored) const;

    /** Convert an absolute path under data/ back to "data/...". */
    QString storeFilename(const QString &absolutePath) const;

private:
    QString m_dataRoot;
};

#endif
