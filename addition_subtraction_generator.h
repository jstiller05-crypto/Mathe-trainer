#ifndef ADDITION_SUBTRACTION_GENERATOR_H
#define ADDITION_SUBTRACTION_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

// Erzeugt eine KOMPLETTE, eigenstaendige Additions-/Subtraktionsaufgabe
Task generateAdditionSubtractionTask(DifficultyLevel level);

// Erzeugt nur EIN Fragment, das eine andere Stelle (z.B. arithmetic_unit.cpp)
// als Operand weiterverwenden kann
TaskFragment generateAdditionSubtractionFragment(DifficultyLevel level);

// Verschmilzt EIN vorhandenes Fragment (z.B. von einem anderen Generator)
// mit einem selbst erzeugten zweiten Wert, per Addition ODER Subtraktion.
// Beispiel-Nutzung: "4² + 8" entsteht daraus, dass "4²" als Fragment reinkommt.
Task combineWithAdditionSubtraction(const TaskFragment &first, DifficultyLevel level);

#endif