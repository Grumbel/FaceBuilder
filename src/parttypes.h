// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef PARTTYPES_H
#define PARTTYPES_H

#include <QPointF>
#include <QString>
#include <QVector>

/** Face-part categories, matching the original Ruby application. */
enum class PartType {
    Eye,
    Eyebrow,
    Glasses,
    Ear,
    Mouth,
    Mouthfold,
    Beard,
    Nose,
    Head,
    Forehead,
    Hair,
    Hat,
    Count
};

inline QString partTypeName(PartType t)
{
    switch (t) {
    case PartType::Eye:      return QStringLiteral("eye");
    case PartType::Eyebrow:  return QStringLiteral("eyebrow");
    case PartType::Glasses:  return QStringLiteral("glasses");
    case PartType::Ear:      return QStringLiteral("ear");
    case PartType::Mouth:    return QStringLiteral("mouth");
    case PartType::Mouthfold:return QStringLiteral("mouthfold");
    case PartType::Beard:    return QStringLiteral("beard");
    case PartType::Nose:     return QStringLiteral("nose");
    case PartType::Head:     return QStringLiteral("head");
    case PartType::Forehead: return QStringLiteral("forehead");
    case PartType::Hair:     return QStringLiteral("hair");
    case PartType::Hat:      return QStringLiteral("hat");
    case PartType::Count:    break;
    }
    return {};
}

/** Directory name under data/ for this type. */
inline QString partTypeDirName(PartType t)
{
    return partTypeName(t);
}

/** Whether this part is drawn twice (left + right mirror). */
inline bool partTypeIsMirrored(PartType t)
{
    switch (t) {
    case PartType::Eye:
    case PartType::Ear:
    case PartType::Eyebrow:
    case PartType::Mouthfold:
        return true;
    default:
        return false;
    }
}

/** Default offset used when a part is first created (matches original). */
inline QPointF partTypeDefaultOffset(PartType t)
{
    switch (t) {
    case PartType::Head:      return {  0,   0};
    case PartType::Ear:       return { 90,   0};
    case PartType::Forehead:  return {  0, -75};
    case PartType::Beard:     return {  0,  75};
    case PartType::Eye:       return { 35, -10};
    case PartType::Eyebrow:   return { 45, -30};
    case PartType::Nose:      return {  0,  30};
    case PartType::Mouth:     return {  0,  75};
    case PartType::Mouthfold: return { 40,  75};
    case PartType::Glasses:   return {  0, -10};
    case PartType::Hair:      return {  0, -20};
    case PartType::Hat:       return {  0, -50};
    case PartType::Count:     break;
    }
    return {};
}

/** Z-order so hair/hat sit above head, eyes above skin, etc. */
inline qreal partTypeZValue(PartType t)
{
    switch (t) {
    case PartType::Head:      return 10;
    case PartType::Forehead:  return 20;
    case PartType::Ear:       return 15;
    case PartType::Eye:       return 40;
    case PartType::Eyebrow:   return 45;
    case PartType::Nose:      return 50;
    case PartType::Mouth:     return 50;
    case PartType::Mouthfold: return 48;
    case PartType::Beard:     return 30;
    case PartType::Glasses:   return 60;
    case PartType::Hair:      return 70;
    case PartType::Hat:       return 80;
    case PartType::Count:     break;
    }
    return 0;
}

/** Order shown in the part-type selector (matches original @parts list). */
inline QVector<PartType> partTypeSelectorOrder()
{
    return {
        PartType::Eye,
        PartType::Eyebrow,
        PartType::Glasses,
        PartType::Ear,
        PartType::Mouth,
        PartType::Mouthfold,
        PartType::Beard,
        PartType::Nose,
        PartType::Head,
        PartType::Forehead,
        PartType::Hair,
        PartType::Hat,
    };
}

#endif // PARTTYPES_H
