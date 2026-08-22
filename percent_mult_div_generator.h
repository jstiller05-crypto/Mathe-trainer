#ifndef PERCENT_MULT_DIV_GENERATOR_H
#define PERCENT_MULT_DIV_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

Task generateMultDivTask(DifficultyLevel level, bool mentalMath);
Task generatePercentTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateMultDivFragment(DifficultyLevel level, bool mentalMath);

#endif