// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef FACESCENE_H
#define FACESCENE_H

#include <QGraphicsScene>

class FaceScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit FaceScene(QObject *parent = nullptr);

    /** Draw a light guide rectangle so the empty canvas is visible. */
    void addGuideFrame();
};

#endif // FACESCENE_H
