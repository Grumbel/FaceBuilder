// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef PARTBROWSER_H
#define PARTBROWSER_H

#include "parttypes.h"

#include <QWidget>

class QComboBox;
class QListWidget;
class QListWidgetItem;

/**
 * Right-hand panel: category combo + thumbnail grid (including a
 * "none" entry). Emits partSelected when the user picks a thumbnail.
 */
class PartBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit PartBrowser(const QString &dataRoot, QWidget *parent = nullptr);

    PartType currentType() const;
    void setCurrentType(PartType type);

signals:
    void currentTypeChanged(PartType type);
    /** Empty path means "none" / clear the slot. */
    void partSelected(PartType type, const QString &filename);

private slots:
    void onCategoryChanged(int index);
    void onThumbnailClicked(QListWidgetItem *item);

private:
    void populateCategories();
    void rebuildThumbnails();
    QPixmap makeThumbnail(const QString &path) const;
    QPixmap noneThumbnail() const;

    QString m_dataRoot;
    QComboBox *m_categoryCombo = nullptr;
    QListWidget *m_thumbnails = nullptr;
    PartType m_currentType = PartType::Eye;
};

#endif // PARTBROWSER_H
