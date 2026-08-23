#ifndef PERCENT_MULT_DIV_GENERATOR_H
#define PERCENT_MULT_DIV_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

Task generateMultiplicationTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateMultiplicationFragment(DifficultyLevel level, bool mentalMath);

Task generateDivisionTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateDivisionFragment(DifficultyLevel level, bool mentalMath);

Task generatePercentTask(DifficultyLevel level, bool mentalMath);

#endif