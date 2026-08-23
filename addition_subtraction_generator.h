#ifndef ADDITION_SUBTRACTION_GENERATOR_H
#define ADDITION_SUBTRACTION_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

Task generateAdditionTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateAdditionFragment(DifficultyLevel level, bool mentalMath);

Task generateSubtractionTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateSubtractionFragment(DifficultyLevel level, bool mentalMath);

TaskFragment combineFragments(const TaskFragment &a, const TaskFragment &b, bool mentalMath);

#endif