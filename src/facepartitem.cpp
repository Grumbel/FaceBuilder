// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "facepartitem.h"
#include "facepart.h"

#include <QGraphicsScene>
#include <QPixmap>
#include <QTransform>

FacePartItem::FacePartItem(PartType type, QGraphicsScene *scene, QObject *parent)
    : QObject(parent)
    , m_type(type)
    , m_scene(scene)
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
}

FacePartItem::~FacePartItem()
{
    // Scene owns the items if still attached; remove explicitly for clarity.
    if (m_primary) {
        m_scene->removeItem(m_primary);
        delete m_primary;
        m_primary = nullptr;
    }
    if (m_mirror) {
        m_scene->removeItem(m_mirror);
        delete m_mirror;
        m_mirror = nullptr;
    }
}

void FacePartItem::applyTransform(QGraphicsPixmapItem *item, const QPointF &offset,
                                  qreal scale, qreal rotation, bool mirror)
{
    if (!item)
        return;

    const QPixmap pm = item->pixmap();
    if (pm.isNull())
        return;

    // Anchor at pixmap centre (matches GnomeCanvas ANCHOR_CENTER).
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
        if (m_mirror)
            m_mirror->setVisible(false);
        return;
    }

    QPixmap pm(part.filename());
    if (pm.isNull()) {
        m_primary->setVisible(false);
        if (m_mirror)
            m_mirror->setVisible(false);
        return;
    }

    m_primary->setPixmap(pm);
    applyTransform(m_primary, part.offset(), part.scale(), part.rotation(), false);
    m_primary->setVisible(true);

    if (m_mirror) {
        m_mirror->setPixmap(pm);
        // Mirror across the vertical axis through the face centre:
        // same offset, but with a horizontal flip applied first.
        applyTransform(m_mirror, part.offset(), part.scale(), part.rotation(), true);
        m_mirror->setVisible(true);
    }
}
