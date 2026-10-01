// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef FACE_H
#define FACE_H

#include "facepart.h"
#include "parttypes.h"

#include <QObject>
#include <array>

/**
 * Owns all face-part slots. Emits signals when a part changes so the
 * graphics layer can update. Undo will be added in M3 via QUndoStack.
 */
class Face : public QObject
{
    Q_OBJECT

public:
    explicit Face(QObject *parent = nullptr);

    FacePart &part(PartType t);
    const FacePart &part(PartType t) const;

    void clearAll();

    /** Shift all offsets so the head is at the origin (original center()). */
    void centerOnHead();

    /** Assign a PNG to a slot (empty path = hide / none). */
    void setPartFilename(PartType t, const QString &path);

    void setPartOffset(PartType t, const QPointF &offset);
    void setPartScale(PartType t, qreal scale);
    void setPartRotation(PartType t, qreal degrees);
    void resetPartTransform(PartType t);

signals:
    void partChanged(PartType type);

private:
    std::array<FacePart, static_cast<size_t>(PartType::Count)> m_parts;
};

#endif // FACE_H
