// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "facescene.h"
#include "face.h"
#include "facepartitem.h"

#include <QGraphicsRectItem>
#include <QPen>

FaceScene::FaceScene(Face *face, QObject *parent)
    : QGraphicsScene(parent)
    , m_face(face)
{
    setSceneRect(-256, -256, 512, 512);
    addGuideFrame();

    for (size_t i = 0; i < static_cast<size_t>(PartType::Count); ++i) {
        auto t = static_cast<PartType>(i);
        m_items[i] = std::make_unique<FacePartItem>(t, this, this);
        m_items[i]->updateFrom(m_face->part(t));
    }

    connect(m_face, &Face::partChanged, this, &FaceScene::onPartChanged);
}

FaceScene::~FaceScene() = default;

void FaceScene::addGuideFrame()
{
    auto *frame = addRect(QRectF(-256, -256, 512, 512),
                          QPen(QColor(220, 220, 220), 1.0),
                          QBrush(Qt::NoBrush));
    frame->setZValue(-1000);
    frame->setFlag(QGraphicsItem::ItemIsSelectable, false);
    frame->setFlag(QGraphicsItem::ItemIsMovable, false);
}

void FaceScene::clearFace()
{
    m_face->clearAll();
}

void FaceScene::onPartChanged(PartType type)
{
    const size_t i = static_cast<size_t>(type);
    if (m_items[i])
        m_items[i]->updateFrom(m_face->part(type));
}
