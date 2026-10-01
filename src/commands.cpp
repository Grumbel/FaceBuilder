// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "commands.h"
#include "face.h"

SetPartFilenameCommand::SetPartFilenameCommand(Face *face, PartType type,
                                               const QString &oldPath, const QString &newPath,
                                               QUndoCommand *parent)
    : QUndoCommand(parent), m_face(face), m_type(type), m_oldPath(oldPath), m_newPath(newPath)
{
    setText(QObject::tr("Change %1").arg(partTypeName(type)));
}
void SetPartFilenameCommand::undo() { m_face->setPartFilename(m_type, m_oldPath); }
void SetPartFilenameCommand::redo() { m_face->setPartFilename(m_type, m_newPath); }

SetPartOffsetCommand::SetPartOffsetCommand(Face *face, PartType type,
                                           const QPointF &oldOffset, const QPointF &newOffset,
                                           QUndoCommand *parent)
    : QUndoCommand(parent), m_face(face), m_type(type), m_oldOffset(oldOffset), m_newOffset(newOffset)
{
    setText(QObject::tr("Move %1").arg(partTypeName(type)));
}
void SetPartOffsetCommand::undo() { m_face->setPartOffset(m_type, m_oldOffset); }
void SetPartOffsetCommand::redo() { m_face->setPartOffset(m_type, m_newOffset); }

SetPartScaleCommand::SetPartScaleCommand(Face *face, PartType type,
                                         qreal oldScale, qreal newScale,
                                         QUndoCommand *parent)
    : QUndoCommand(parent), m_face(face), m_type(type), m_oldScale(oldScale), m_newScale(newScale)
{
    setText(QObject::tr("Scale %1").arg(partTypeName(type)));
}
void SetPartScaleCommand::undo() { m_face->setPartScale(m_type, m_oldScale); }
void SetPartScaleCommand::redo() { m_face->setPartScale(m_type, m_newScale); }

SetPartRotationCommand::SetPartRotationCommand(Face *face, PartType type,
                                               qreal oldRotation, qreal newRotation,
                                               QUndoCommand *parent)
    : QUndoCommand(parent), m_face(face), m_type(type), m_oldRotation(oldRotation), m_newRotation(newRotation)
{
    setText(QObject::tr("Rotate %1").arg(partTypeName(type)));
}
void SetPartRotationCommand::undo() { m_face->setPartRotation(m_type, m_oldRotation); }
void SetPartRotationCommand::redo() { m_face->setPartRotation(m_type, m_newRotation); }

ResetPartTransformCommand::ResetPartTransformCommand(Face *face, PartType type,
                                                     qreal oldScale, qreal oldRotation,
                                                     QUndoCommand *parent)
    : QUndoCommand(parent), m_face(face), m_type(type), m_oldScale(oldScale), m_oldRotation(oldRotation)
{
    setText(QObject::tr("Reset %1").arg(partTypeName(type)));
}
void ResetPartTransformCommand::undo()
{
    m_face->setPartScale(m_type, m_oldScale);
    m_face->setPartRotation(m_type, m_oldRotation);
}
void ResetPartTransformCommand::redo() { m_face->resetPartTransform(m_type); }

CenterFaceCommand::CenterFaceCommand(Face *face, QUndoCommand *parent)
    : QUndoCommand(parent), m_face(face)
{
    setText(QObject::tr("Center face"));
    const QPointF head = face->part(PartType::Head).offset();
    for (size_t i = 0; i < static_cast<size_t>(PartType::Count); ++i) {
        auto t = static_cast<PartType>(i);
        m_oldOffsets[i] = face->part(t).offset();
        m_newOffsets[i] = QPointF(m_oldOffsets[i].x() - head.x(),
                                  m_oldOffsets[i].y() - head.y());
    }
}

void CenterFaceCommand::undo()
{
    for (size_t i = 0; i < static_cast<size_t>(PartType::Count); ++i)
        m_face->setPartOffset(static_cast<PartType>(i), m_oldOffsets[i]);
}

void CenterFaceCommand::redo()
{
    for (size_t i = 0; i < static_cast<size_t>(PartType::Count); ++i)
        m_face->setPartOffset(static_cast<PartType>(i), m_newOffsets[i]);
}
