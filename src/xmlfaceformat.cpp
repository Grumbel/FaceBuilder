// SPDX-License-Identifier: GPL-3.0-or-later
#include <QObject>
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "xmlfaceformat.h"
#include "face.h"
#include "parttypes.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

XmlFaceFormat::XmlFaceFormat(const QString &dataRoot)
    : m_dataRoot(QDir(dataRoot).absolutePath())
{
}

QString XmlFaceFormat::resolveFilename(const QString &stored) const
{
    if (stored.isEmpty())
        return {};

    // Typical form: "data/head/0002.png"
    QString rel = stored;
    if (rel.startsWith(QStringLiteral("data/")))
        rel = rel.mid(5);

    const QString abs = QDir(m_dataRoot).absoluteFilePath(rel);
    if (QFile::exists(abs))
        return abs;

    // Already absolute?
    if (QFile::exists(stored))
        return stored;

    return abs; // return expected path even if missing (caller may warn)
}

QString XmlFaceFormat::storeFilename(const QString &absolutePath) const
{
    if (absolutePath.isEmpty())
        return {};

    const QString abs = QDir::cleanPath(absolutePath);
    const QString root = QDir::cleanPath(m_dataRoot);

    if (abs.startsWith(root)) {
        QString rel = abs.mid(root.size());
        if (rel.startsWith(QLatin1Char('/')))
            rel = rel.mid(1);
        return QStringLiteral("data/") + rel;
    }

    // Fallback: keep basename under unknown type
    return QStringLiteral("data/") + QFileInfo(abs).fileName();
}

bool XmlFaceFormat::load(Face *face, const QString &filePath, QString *error) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error)
            *error = QObject::tr("Cannot open %1").arg(filePath);
        return false;
    }

    // Reset all slots first (matches original load behaviour).
    face->clearAll();

    QXmlStreamReader xml(&file);
    PartType current = PartType::Count;
    QString filename;
    qreal ox = 0, oy = 0, scale = 1.0, rotation = 0.0;
    bool inOffset = false;
    QString offsetField;

    auto commitPart = [&]() {
        if (current == PartType::Count)
            return;
        const QString abs = resolveFilename(filename);
        face->setPartFilename(current, abs);
        face->setPartOffset(current, QPointF(ox, oy));
        face->setPartScale(current, scale);
        face->setPartRotation(current, rotation);
        current = PartType::Count;
        filename.clear();
        ox = oy = 0;
        scale = 1.0;
        rotation = 0.0;
    };

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            const QString name = xml.name().toString();
            if (name == QLatin1String("face")) {
                continue;
            }
            if (name == QLatin1String("filename")) {
                filename = xml.readElementText().trimmed();
            } else if (name == QLatin1String("offset")) {
                inOffset = true;
            } else if (inOffset && name == QLatin1String("x")) {
                ox = xml.readElementText().toDouble();
            } else if (inOffset && name == QLatin1String("y")) {
                oy = xml.readElementText().toDouble();
            } else if (name == QLatin1String("scale")) {
                scale = xml.readElementText().toDouble();
            } else if (name == QLatin1String("rotation")) {
                rotation = xml.readElementText().toDouble();
            } else {
                // Part element name matches directory name.
                for (int i = 0; i < static_cast<int>(PartType::Count); ++i) {
                    auto t = static_cast<PartType>(i);
                    if (partTypeName(t) == name) {
                        current = t;
                        filename.clear();
                        ox = oy = 0;
                        scale = 1.0;
                        rotation = 0.0;
                        break;
                    }
                }
            }
        } else if (xml.isEndElement()) {
            const QString name = xml.name().toString();
            if (name == QLatin1String("offset")) {
                inOffset = false;
            } else if (current != PartType::Count && partTypeName(current) == name) {
                commitPart();
            }
        }
    }

    if (xml.hasError()) {
        if (error)
            *error = xml.errorString();
        return false;
    }
    return true;
}

bool XmlFaceFormat::save(const Face *face, const QString &filePath, QString *error) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (error)
            *error = QObject::tr("Cannot write %1").arg(filePath);
        return false;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.writeStartDocument();
    xml.writeStartElement(QStringLiteral("face"));

    for (int i = 0; i < static_cast<int>(PartType::Count); ++i) {
        auto t = static_cast<PartType>(i);
        const FacePart &p = face->part(t);
        if (p.filename().isEmpty())
            continue;

        xml.writeStartElement(partTypeName(t));
        xml.writeTextElement(QStringLiteral("filename"), storeFilename(p.filename()));
        xml.writeStartElement(QStringLiteral("offset"));
        xml.writeTextElement(QStringLiteral("x"), QString::number(p.offset().x()));
        xml.writeTextElement(QStringLiteral("y"), QString::number(p.offset().y()));
        xml.writeEndElement(); // offset
        xml.writeTextElement(QStringLiteral("scale"), QString::number(p.scale(), 'g', 15));
        xml.writeTextElement(QStringLiteral("rotation"), QString::number(p.rotation(), 'g', 15));
        xml.writeEndElement(); // part
    }

    xml.writeEndElement(); // face
    xml.writeEndDocument();
    return true;
}
