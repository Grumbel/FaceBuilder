// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef COMMANDS_H
#define COMMANDS_H

#include "parttypes.h"

#include <QPointF>
#include <QString>
#include <QUndoCommand>

class Face;

class SetPartFilenameCommand : public QUndoCommand
{
public:
    SetPartFilenameCommand(Face *face, PartType type,
                           const QString &oldPath, const QString &newPath,
                           QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Face *m_face;
    PartType m_type;
    QString m_oldPath;
    QString m_newPath;
};

class SetPartOffsetCommand : public QUndoCommand
{
public:
    SetPartOffsetCommand(Face *face, PartType type,
                         const QPointF &oldOffset, const QPointF &newOffset,
                         QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Face *m_face;
    PartType m_type;
    QPointF m_oldOffset;
    QPointF m_newOffset;
};

class SetPartScaleCommand : public QUndoCommand
{
public:
    SetPartScaleCommand(Face *face, PartType type,
                        qreal oldScale, qreal newScale,
                        QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Face *m_face;
    PartType m_type;
    qreal m_oldScale;
    qreal m_newScale;
};

class SetPartRotationCommand : public QUndoCommand
{
public:
    SetPartRotationCommand(Face *face, PartType type,
                           qreal oldRotation, qreal newRotation,
                           QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Face *m_face;
    PartType m_type;
    qreal m_oldRotation;
    qreal m_newRotation;
};

class ResetPartTransformCommand : public QUndoCommand
{
public:
    ResetPartTransformCommand(Face *face, PartType type,
                              qreal oldScale, qreal oldRotation,
                              QUndoCommand *parent = nullptr);
    void undo() override;
    void redo() override;

private:
    Face *m_face;
    PartType m_type;
    qreal m_oldScale;
    qreal m_oldRotation;
};

#endif // COMMANDS_H
