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

private:
    Task currentTask;
    DifficultyLevel currentLevel;
};

#endif // SESSION_CONTROLLER_H
