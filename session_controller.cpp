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

QVector<bool> SessionController::checkAnswers(const QVector<QString> &inputs) const
{
    QVector<bool> results;

    for (int i = 0; i < currentTask.answers.size(); ++i) {
        bool isNumber = false;
        double given = (i < inputs.size()) ? inputs[i].toDouble(&isNumber) : 0.0;

        // kleine Toleranz statt exaktem Vergleich - wichtig für Dezimalzahlen (Rundungsfehler)
        bool correct = isNumber && qAbs(given - currentTask.answers[i].expectedValue) < 0.001;
        results.append(correct);
    }

    return results;
}