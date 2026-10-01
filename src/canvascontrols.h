// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef CANVASCONTROLS_H
#define CANVASCONTROLS_H

#include "parttypes.h"

#include <QObject>
#include <QVector>

class QGraphicsScene;
class QGraphicsProxyWidget;

/**
 * On-canvas <- / -> buttons around the face (original setup_controls).
 * Clicking cycles the PNG for that part type and selects it.
 */
class CanvasControls : public QObject
{
    Q_OBJECT

public:
    CanvasControls(QGraphicsScene *scene, QObject *parent = nullptr);

    void setVisible(bool visible);
    bool isVisible() const { return m_visible; }

signals:
    void cyclePart(PartType type, int delta);

private:
    void addPair(PartType type, const QPointF &pos);

    QGraphicsScene *m_scene = nullptr;
    QVector<QGraphicsProxyWidget *> m_proxies;
    bool m_visible = false;
};

#endif
