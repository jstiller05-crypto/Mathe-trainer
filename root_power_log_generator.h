#ifndef ROOT_POWER_LOG_GENERATOR_H
#define ROOT_POWER_LOG_GENERATOR_H

#include "task.h"
#include "task_fragment.h"
#include "difficulty.h"

Task generateRootPowerLogTask(DifficultyLevel level, bool mentalMath);
TaskFragment generateRootPowerLogFragment(DifficultyLevel level, bool mentalMath);

#endif
