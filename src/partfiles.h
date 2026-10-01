// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef PARTFILES_H
#define PARTFILES_H

#include "parttypes.h"

#include <QString>
#include <QStringList>

/** Sorted list of absolute PNG paths under dataRoot/<type>/. */
QStringList listPartFiles(const QString &dataRoot, PartType type);

/**
 * Next/previous PNG in the category (wraps). If current is empty or not
 * in the list, starts at first (delta>0) or last (delta<0).
 * Returns empty if the category has no files.
 */
QString cyclePartFile(const QString &dataRoot, PartType type,
                      const QString &currentAbsolute, int delta);

#endif
