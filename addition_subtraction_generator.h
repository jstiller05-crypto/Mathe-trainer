#ifndef ADDITION_SUBTRACTION_GENERATOR_H
#define ADDITION_SUBTRACTION_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

Task generateAdditionSubtractionTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateAdditionSubtractionFragment(DifficultyLevel level, bool mentalMath);
Task combineWithAdditionSubtraction(const TaskFragment &first, DifficultyLevel level, bool mentalMath);
TaskFragment combineFragments(const TaskFragment &a, const TaskFragment &b, bool mentalMath);

#endif