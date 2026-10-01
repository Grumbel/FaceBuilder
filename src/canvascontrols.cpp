// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "canvascontrols.h"

#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QToolButton>
#include <QHBoxLayout>
#include <QWidget>

CanvasControls::CanvasControls(QGraphicsScene *scene, QObject *parent)
    : QObject(parent)
    , m_scene(scene)
{
    // Positions roughly match the original Ruby layout (scene coords).
    addPair(PartType::Hat,      QPointF(0, -200));
    addPair(PartType::Forehead, QPointF(-200, -140));
    addPair(PartType::Hair,     QPointF(200, -140));
    addPair(PartType::Glasses,  QPointF(-200, 0));
    addPair(PartType::Eyebrow,  QPointF(200, -40));
    addPair(PartType::Eye,      QPointF(200, 0));
    addPair(PartType::Nose,     QPointF(200, 40));
    addPair(PartType::Ear,      QPointF(-200, 40));
    addPair(PartType::Mouth,    QPointF(200, 80));
    addPair(PartType::Mouthfold,QPointF(-200, 80));
    addPair(PartType::Head,     QPointF(0, 200));
    addPair(PartType::Beard,    QPointF(0, 230));

    setVisible(false);
}

void CanvasControls::addPair(PartType type, const QPointF &pos)
{
    auto *host = new QWidget();
    host->setAttribute(Qt::WA_TranslucentBackground);
    auto *lay = new QHBoxLayout(host);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(2);

    auto *prev = new QToolButton(host);
    prev->setText(QStringLiteral("←"));
    prev->setFixedSize(28, 24);
    prev->setToolTip(QObject::tr("Previous %1").arg(partTypeName(type)));
    auto *next = new QToolButton(host);
    next->setText(QStringLiteral("→"));
    next->setFixedSize(28, 24);
    next->setToolTip(QObject::tr("Next %1").arg(partTypeName(type)));
    lay->addWidget(prev);
    lay->addWidget(next);

    QObject::connect(prev, &QToolButton::clicked, this, [this, type]() {
        emit cyclePart(type, -1);
    });
    QObject::connect(next, &QToolButton::clicked, this, [this, type]() {
        emit cyclePart(type, +1);
    });

    auto *proxy = m_scene->addWidget(host);
    proxy->setZValue(500);
    proxy->setPos(pos.x() - host->sizeHint().width() / 2.0,
                  pos.y() - host->sizeHint().height() / 2.0);
    proxy->setVisible(false);
    m_proxies.append(proxy);
}

void CanvasControls::setVisible(bool visible)
{
    m_visible = visible;
    for (auto *p : m_proxies)
        p->setVisible(visible);
}
