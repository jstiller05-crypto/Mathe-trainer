#include "session_controller.h"
#include "task_generator.h"
#include <QDebug>

// QString::toDouble() arbeitet IMMER in der C-Locale (Punkt als Dezimaltrennzeichen) -
// das ist unabhaengig davon, welche Sprache/Region in Windows eingestellt ist. Ein
// deutsches Komma wie in "2,35" waere damit ungueltig und die Antwort faelschlich
// "falsch". Die naheliegende Alternative QLocale::system().toDouble() wuerde das
// Komma zwar auch verstehen, behandelt daneben aber z.B. auch Tausendertrennzeichen
// ("2.000") nach den Regeln der Systemsprache - hier reicht das einfache Ersetzen
// von Komma zu Punkt, weil wir nur einzelne, kurze Zahlen ohne Tausendertrennzeichen
// erwarten.
static double parseUserNumber(const QString &text, bool *ok)
{
    QString normalized = text.trimmed();
    normalized.replace(',', '.');
    return normalized.toDouble(ok);
}

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

void SessionController::setTaskMode(TaskMode mode)
{
    taskMode = mode;
    qDebug() << "[SessionController] Aufgaben-Modus gesetzt:" << static_cast<int>(mode);
}

void SessionController::setActiveSelections(const QVector<QPair<QString, QString>> &selections)
{
    activeSelections = selections;
    qDebug() << "[SessionController] Aktive Auswahl gesetzt:" << selections;
}

void SessionController::startNewTask()
{
    currentTask = generateTask(currentLevel, activeSelections, taskMode);
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
        double given = (i < inputs.size()) ? parseUserNumber(inputs[i], &isNumber) : 0.0;

        // kleine Toleranz statt exaktem Vergleich - wichtig für Dezimalzahlen (Rundungsfehler)
        bool correct = isNumber && qAbs(given - currentTask.answers[i].expectedValue) < 0.001;
        results.append(correct);
    }

    return results;
}