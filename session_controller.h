#ifndef SESSION_CONTROLLER_H
#define SESSION_CONTROLLER_H

#include "task.h"
#include "difficulty.h"

// Der Controller kennt die aktuelle Aufgabe und die Regeln,
// aber weiß NICHTS von Qt, Fenstern oder Buttons.
class SessionController
{
public:
    explicit SessionController(DifficultyLevel level = Preset::Beginner);

    void startNewTask();
    Task getCurrentTask() const;
    QVector<bool> checkAnswers(const QVector<QString> &inputs) const;

    void setDifficultyLevel(DifficultyLevel level);   // <- neu, für den späteren Regler
    void setCategory(const QString &category, const QString &subcategory);
    void setMentalMathMode(bool enabled);
    void setActiveSelections(const QVector<QPair<QString, QString>> &selections);

private:
    Task currentTask;
    DifficultyLevel currentLevel;
    QString currentCategory = "Arithmetik";        // Startwert, damit es auch ohne Sidebar-Klick funktioniert
    QString currentSubcategory = "Kopfrechenaufgaben";
    QVector<QPair<QString, QString>> activeSelections = { { "Arithmetik", "Addition & Subtraktion" } };
    bool mentalMathMode = true;
};

#endif // SESSION_CONTROLLER_H
