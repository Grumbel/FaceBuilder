// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef FACESCENE_H
#define FACESCENE_H

#include "parttypes.h"

#include <QGraphicsScene>
#include <array>
#include <memory>

class Face;
class FacePartItem;

class FaceScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit FaceScene(Face *face, QObject *parent = nullptr);
    ~FaceScene() override;

    Face *face() const { return m_face; }

    void addGuideFrame();
    void clearFace();

public slots:
    void onPartChanged(PartType type);

private:
    Face *m_face = nullptr;
    std::array<std::unique_ptr<FacePartItem>, static_cast<size_t>(PartType::Count)> m_items;
};

#endif // FACESCENE_H
