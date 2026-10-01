// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "facescene.h"
#include "face.h"
#include "facepartitem.h"

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

FaceScene::FaceScene(Face *face, QObject *parent)
    : QGraphicsScene(parent), m_face(face)
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
                          QPen(QColor(220, 220, 220), 1.0), QBrush(Qt::NoBrush));
    frame->setZValue(-1000);
    frame->setFlag(QGraphicsItem::ItemIsSelectable, false);
    frame->setFlag(QGraphicsItem::ItemIsMovable, false);
}

void FaceScene::clearFace() { m_face->clearAll(); }

void FaceScene::setCurrentType(PartType type)
{
    if (m_currentType == type) return;
    m_currentType = type;
    for (size_t i = 0; i < static_cast<size_t>(PartType::Count); ++i)
        m_items[i]->setSelected(static_cast<PartType>(i) == type);
    emit currentTypeChanged(type);
}

PartType FaceScene::partAt(const QPointF &scenePos) const
{
    PartType best = PartType::Count;
    qreal bestZ = -1e9;
    for (size_t i = 0; i < static_cast<size_t>(PartType::Count); ++i) {
        auto t = static_cast<PartType>(i);
        if (!m_items[i] || !m_items[i]->isVisible()) continue;
        if (!m_items[i]->containsScenePos(scenePos)) continue;
        const qreal z = partTypeZValue(t);
        if (z >= bestZ) { bestZ = z; best = t; }
    }
    return best;
}

void FaceScene::onPartChanged(PartType type)
{
    const size_t i = static_cast<size_t>(type);
    if (m_items[i])
        m_items[i]->updateFrom(m_face->part(type));
}

void FaceScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const PartType hit = partAt(event->scenePos());
        if (hit != PartType::Count) {
            setCurrentType(hit);
            m_dragging = true;
            m_dragType = hit;
            m_dragStartScene = event->scenePos();
            m_dragStartOffset = m_face->part(hit).offset();
            event->accept();
            return;
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void FaceScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_dragging && m_dragType != PartType::Count
        && (event->buttons() & Qt::LeftButton)) {
        const QPointF delta = event->scenePos() - m_dragStartScene;
        QPointF newOffset = m_dragStartOffset + delta;
        if (event->modifiers() & Qt::ShiftModifier)
            newOffset.setX(m_dragStartOffset.x());
        m_face->setPartOffset(m_dragType, newOffset);
        event->accept();
        return;
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void FaceScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_dragging && event->button() == Qt::LeftButton) {
        const QPointF finalOffset = m_face->part(m_dragType).offset();
        if (finalOffset != m_dragStartOffset)
            emit partMoved(m_dragType, m_dragStartOffset, finalOffset);
        m_dragging = false;
        m_dragType = PartType::Count;
        event->accept();
        return;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}
