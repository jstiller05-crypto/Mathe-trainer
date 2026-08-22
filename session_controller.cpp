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

void SessionController::setMentalMathMode(bool enabled)
{
    mentalMathMode = enabled;
    qDebug() << "[SessionController] Kopfrechnen-Modus gesetzt:" << enabled;
}

void SessionController::setActiveSelections(const QVector<QPair<QString, QString>> &selections)
{
    activeSelections = selections;
    qDebug() << "[SessionController] Aktive Auswahl gesetzt:" << selections;
}

void SessionController::startNewTask()
{
    currentTask = generateTask(currentLevel, activeSelections, mentalMathMode);
}

Task SessionController::getCurrentTask() const
{
    return currentTask;
}

void SessionController::setCategory(const QString &category, const QString &subcategory)
{
    currentCategory = category;
    currentSubcategory = subcategory;
    qDebug() << "[SessionController] Kategorie geaendert:" << category << "-" << subcategory;
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