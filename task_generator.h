// task_generator.h
#ifndef TASK_GENERATOR_H
#define TASK_GENERATOR_H

#include "task.h"
#include "difficulty.h"
#include <QString>

Task generateTask(DifficultyLevel level, const QVector<QPair<QString, QString>> &activeSelections, TaskMode mode);

#endif