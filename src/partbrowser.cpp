// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "partbrowser.h"

#include <QComboBox>
#include <QDir>
#include <QIcon>
#include <QListWidget>
#include <QPainter>
#include <QVBoxLayout>

static constexpr int kThumbSize = 64;

PartBrowser::PartBrowser(const QString &dataRoot, QWidget *parent)
    : QWidget(parent)
    , m_dataRoot(dataRoot)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_categoryCombo = new QComboBox(this);
    layout->addWidget(m_categoryCombo);

    m_thumbnails = new QListWidget(this);
    m_thumbnails->setViewMode(QListWidget::IconMode);
    m_thumbnails->setIconSize(QSize(kThumbSize, kThumbSize));
    m_thumbnails->setResizeMode(QListWidget::Adjust);
    m_thumbnails->setMovement(QListWidget::Static);
    m_thumbnails->setSpacing(4);
    m_thumbnails->setUniformItemSizes(true);
    layout->addWidget(m_thumbnails, /*stretch=*/1);

    populateCategories();
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PartBrowser::onCategoryChanged);
    connect(m_thumbnails, &QListWidget::itemClicked,
            this, &PartBrowser::onThumbnailClicked);

    rebuildThumbnails();
}

PartType PartBrowser::currentType() const
{
    return m_currentType;
}

void PartBrowser::setCurrentType(PartType type)
{
    const auto order = partTypeSelectorOrder();
    const int idx = order.indexOf(type);
    if (idx >= 0)
        m_categoryCombo->setCurrentIndex(idx);
}

void PartBrowser::populateCategories()
{
    m_categoryCombo->clear();
    for (PartType t : partTypeSelectorOrder())
        m_categoryCombo->addItem(partTypeName(t), QVariant::fromValue(static_cast<int>(t)));
}

void PartBrowser::onCategoryChanged(int index)
{
    if (index < 0)
        return;
    m_currentType = static_cast<PartType>(m_categoryCombo->itemData(index).toInt());
    rebuildThumbnails();
    emit currentTypeChanged(m_currentType);
}

void PartBrowser::onThumbnailClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    const QString path = item->data(Qt::UserRole).toString();
    emit partSelected(m_currentType, path);
}

QPixmap PartBrowser::noneThumbnail() const
{
    // Red X on a light background — matches the original empty.png look.
    QPixmap pm(kThumbSize, kThumbSize);
    pm.fill(QColor(0xf5, 0xf5, 0xf5));
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    QPen pen(QColor(220, 40, 40), 5, Qt::SolidLine, Qt::RoundCap);
    p.setPen(pen);
    const int m = 14;
    p.drawLine(m, m, kThumbSize - m, kThumbSize - m);
    p.drawLine(kThumbSize - m, m, m, kThumbSize - m);
    p.end();
    return pm;
}

QPixmap PartBrowser::makeThumbnail(const QString &path) const
{
    QPixmap src(path);
    if (src.isNull())
        return noneThumbnail();

    // Scale preserving aspect, centre on a light 64×64 tile.
    QPixmap scaled = src.scaled(kThumbSize, kThumbSize, Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    QPixmap tile(kThumbSize, kThumbSize);
    tile.fill(QColor(0xf5, 0xf5, 0xf5));
    QPainter p(&tile);
    p.drawPixmap((kThumbSize - scaled.width()) / 2,
                 (kThumbSize - scaled.height()) / 2,
                 scaled);
    p.end();
    return tile;
}

void PartBrowser::rebuildThumbnails()
{
    m_thumbnails->clear();

    // "None" entry first.
    auto *noneItem = new QListWidgetItem(QIcon(noneThumbnail()), QString());
    noneItem->setData(Qt::UserRole, QString()); // empty path
    noneItem->setToolTip(tr("None"));
    noneItem->setSizeHint(QSize(kThumbSize + 8, kThumbSize + 8));
    m_thumbnails->addItem(noneItem);

    const QString dirPath = m_dataRoot + QLatin1Char('/') + partTypeDirName(m_currentType);
    QDir dir(dirPath);
    if (!dir.exists())
        return;

    const QStringList files = dir.entryList({QStringLiteral("*.png")},
                                            QDir::Files, QDir::Name);
    for (const QString &name : files) {
        const QString full = dir.absoluteFilePath(name);
        auto *item = new QListWidgetItem(QIcon(makeThumbnail(full)), QString());
        item->setData(Qt::UserRole, full);
        item->setToolTip(name);
        item->setSizeHint(QSize(kThumbSize + 8, kThumbSize + 8));
        m_thumbnails->addItem(item);
    }
}
