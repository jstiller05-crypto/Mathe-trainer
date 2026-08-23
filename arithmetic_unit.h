#ifndef ARITHMETIC_UNIT_H
#define ARITHMETIC_UNIT_H

#include "task.h"
#include "difficulty.h"
#include <QStringList>

Task generateArithmeticTask(DifficultyLevel level, const QStringList &activeSubcategories, bool mentalMath);
QStringList arithmeticAvailableSubcategories(DifficultyLevel level);

#endif