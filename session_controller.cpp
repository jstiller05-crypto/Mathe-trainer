#include "session_controller.h"
#include "task_generator.h"
#include <QDebug>

SessionController::SessionController(Difficulty difficulty)
    : currentDifficulty(difficulty)
{
    qDebug() << "SessionController created with difficulty:" << static_cast<int>(difficulty);
    startNewTask();
}

void SessionController::startNewTask()
{
    currentTask = generateTask(currentDifficulty);
    qDebug() << "New task:" << currentTask.firstNumber
             << static_cast<int>(currentTask.operation)
             << currentTask.secondNumber << "= " << currentTask.solution;
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