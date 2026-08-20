#ifndef ARITHMETIC_UNIT_H
#define ARITHMETIC_UNIT_H

#include "task.h"
#include "difficulty.h"

// Eingangspunkt fuer die komplette Kategorie "Arithmetik" - entscheidet,
// welcher Einzel-Generator (oder welche Kombination davon) benutzt wird.
Task generateArithmeticTask(DifficultyLevel level);

#endif