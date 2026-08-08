#ifndef SESSION_CONTROLLER_H
#define SESSION_CONTROLLER_H

#include "task.h"
#include "difficulty.h"

// Der Controller kennt die aktuelle Aufgabe und die Regeln,
// aber weiß NICHTS von Qt, Fenstern oder Buttons.
class SessionController
{
public:
    explicit SessionController(Difficulty difficulty = Difficulty::Beginner);

    // Erzeugt eine neue Aufgabe und merkt sie sich intern
    void startNewTask();

    // Gibt die aktuell aktive Aufgabe zurück, damit die UI sie anzeigen kann
    Task getCurrentTask() const;

    // Prüft eine gegebene Antwort gegen die aktuelle Aufgabe
    bool checkAnswer(int answer) const;

private:
    Task currentTask;
    Difficulty currentDifficulty;
};

#endif // SESSION_CONTROLLER_H
