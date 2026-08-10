#include "session_controller.h"
#include "task_generator.h"
#include <QDebug>

// session_controller.cpp
SessionController::SessionController(DifficultyLevel level)
    : currentLevel(level)
{
    startNewTask();
}

void SessionController::setDifficultyLevel(DifficultyLevel level)
{
    currentLevel = level;
    qDebug() << "Difficulty level changed to:" << level;
}

void SessionController::startNewTask()
{
    currentTask = generateTask(currentLevel);
    // ... restlicher qDebug-Code bleibt gleich
}

Task SessionController::getCurrentTask() const
{
    return currentTask;
}

bool SessionController::checkAnswer(int answer) const
{
    bool correct = (answer == currentTask.solution);
    qDebug() << "Answer given:" << answer << "| Expected:" << currentTask.solution << "| Correct:" << correct;
    return answer == currentTask.solution;
}