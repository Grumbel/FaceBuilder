// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "facepartitem.h"
#include "facepart.h"

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPen>
#include <QPixmap>
#include <QTransform>

FacePartItem::FacePartItem(PartType type, QGraphicsScene *scene, QObject *parent)
    : QObject(parent), m_type(type), m_scene(scene)
{
    m_primary = new QGraphicsPixmapItem();
    m_primary->setTransformationMode(Qt::SmoothTransformation);
    m_primary->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    m_primary->setZValue(partTypeZValue(type));
    m_primary->setVisible(false);
    m_scene->addItem(m_primary);

    if (partTypeIsMirrored(type)) {
        m_mirror = new QGraphicsPixmapItem();
        m_mirror->setTransformationMode(Qt::SmoothTransformation);
        m_mirror->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
        m_mirror->setZValue(partTypeZValue(type));
        m_mirror->setVisible(false);
        m_scene->addItem(m_mirror);
    }

    m_selRect = new QGraphicsRectItem();
    m_selRect->setPen(QPen(QColor(60, 140, 220), 1.5, Qt::DashLine));
    m_selRect->setBrush(Qt::NoBrush);
    m_selRect->setZValue(partTypeZValue(type) + 0.5);
    m_selRect->setVisible(false);
    m_scene->addItem(m_selRect);
}

FacePartItem::~FacePartItem()
{
    auto remove = [this](QGraphicsItem *item) {
        if (item) { m_scene->removeItem(item); delete item; }
    };
    remove(m_primary); m_primary = nullptr;
    remove(m_mirror); m_mirror = nullptr;
    remove(m_selRect); m_selRect = nullptr;
}

void FacePartItem::applyTransform(QGraphicsPixmapItem *item, const QPointF &offset,
                                  qreal scale, qreal rotation, bool mirror)
{
    if (!item) return;
    const QPixmap pm = item->pixmap();
    if (pm.isNull()) return;

    item->setOffset(-pm.width() / 2.0, -pm.height() / 2.0);

    QTransform t;
    if (mirror)
        t.scale(-1.0, 1.0);
    t.translate(offset.x(), offset.y());
    t.rotate(rotation);
    t.scale(scale, scale);
    item->setTransform(t);
    item->setPos(0, 0);
}

void FacePartItem::updateFrom(const FacePart &part)
{
    if (part.filename().isEmpty()) {
        m_primary->setVisible(false);
        if (m_mirror) m_mirror->setVisible(false);
        updateSelectionVisual();
        return;
    }

    QPixmap pm(part.filename());
    if (pm.isNull()) {
        m_primary->setVisible(false);
        if (m_mirror) m_mirror->setVisible(false);
        updateSelectionVisual();
        return;
    }

    m_primary->setPixmap(pm);
    applyTransform(m_primary, part.offset(), part.scale(), part.rotation(), false);
    m_primary->setVisible(true);

    if (m_mirror) {
        m_mirror->setPixmap(pm);
        applyTransform(m_mirror, part.offset(), part.scale(), part.rotation(), true);
        m_mirror->setVisible(true);
    }
    updateSelectionVisual();
}

bool FacePartItem::containsScenePos(const QPointF &scenePos) const
{
    if (m_primary && m_primary->isVisible()) {
        if (m_primary->contains(m_primary->mapFromScene(scenePos)))
            return true;
    }
    if (m_mirror && m_mirror->isVisible()) {
        if (m_mirror->contains(m_mirror->mapFromScene(scenePos)))
            return true;
    }
    return false;
}

void FacePartItem::setSelected(bool selected)
{
    if (m_selected == selected) return;
    m_selected = selected;
    updateSelectionVisual();
}

bool FacePartItem::isVisible() const
{
    return (m_primary && m_primary->isVisible())
        || (m_mirror && m_mirror->isVisible());
}

void FacePartItem::updateSelectionVisual()
{
    if (!m_selRect) return;
    if (!m_selected || !isVisible()) {
        m_selRect->setVisible(false);
        return;
    }
    QRectF r;
    if (m_primary && m_primary->isVisible())
        r = m_primary->sceneBoundingRect();
    if (m_mirror && m_mirror->isVisible())
        r = r.united(m_mirror->sceneBoundingRect());
    m_selRect->setRect(r.adjusted(-2, -2, 2, 2));
    m_selRect->setTransform(QTransform());
    m_selRect->setPos(0, 0);
    m_selRect->setVisible(true);
}
