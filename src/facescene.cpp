// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "facescene.h"

#include <QGraphicsRectItem>
#include <QPen>

FaceScene::FaceScene(QObject *parent)
    : QGraphicsScene(parent)
{
    setSceneRect(-256, -256, 512, 512);
    addGuideFrame();
}

void FaceScene::addGuideFrame()
{
    // Subtle border so an empty white scene is still obvious.
    auto *frame = addRect(QRectF(-256, -256, 512, 512),
                          QPen(QColor(220, 220, 220), 1.0),
                          QBrush(Qt::NoBrush));
    frame->setZValue(-1000);
    frame->setFlag(QGraphicsItem::ItemIsSelectable, false);
    frame->setFlag(QGraphicsItem::ItemIsMovable, false);
}
