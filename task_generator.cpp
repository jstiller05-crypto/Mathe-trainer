#include "task_generator.h"
#include <cstdlib>
#include <cmath>

int solve(int first, int second, Operation operation)
{
    switch (operation) {
    case Operation::Addition:       return first + second;
    case Operation::Subtraction:    return first - second;
    case Operation::Multiplication: return first * second;
    case Operation::Power:          return static_cast<int>(std::pow(first, second));
    }
    return 0;
}

// Bestimmt den maximalen Zahlenbereich abhängig vom Level.
// Level 1 -> Zahlen bis 10, Level 100 -> Zahlen bis ca. 100
int maxNumberForLevel(DifficultyLevel level)
{
    return 10 + level;   // einfache lineare Skalierung, feinjustierbar
}

// Bestimmt, welche Operation bei diesem Level erlaubt ist -
// je höher der Level, desto mehr/schwerere Operationen kommen dazu
Operation pickOperationForLevel(DifficultyLevel level)
{
    if (level < 20) {
        // nur Addition/Subtraktion
        return (rand() % 2 == 0) ? Operation::Addition : Operation::Subtraction;
    } else if (level < 60) {
        // + - *, gleichmäßig verteilt
        int choice = rand() % 3;
        if (choice == 0) return Operation::Addition;
        if (choice == 1) return Operation::Subtraction;
        return Operation::Multiplication;
    } else {
        // + - * ^, Potenzen erst ab hohem Level
        int choice = rand() % 10;
        if (choice < 3) return Operation::Addition;
        if (choice < 6) return Operation::Subtraction;
        if (choice < 9) return Operation::Multiplication;
        return Operation::Power;   // nur 1 von 10 Fällen, kommt seltener vor
    }
}

Task generateTask(DifficultyLevel level)
{
    Task task;
    int maxNumber = maxNumberForLevel(level);

    task.operation = pickOperationForLevel(level);
    task.firstNumber = rand() % maxNumber + 1;
    task.secondNumber = rand() % maxNumber + 1;

    // Sonderregel: bei Potenzen den Exponenten klein halten, egal wie hoch das Level ist
    if (task.operation == Operation::Power) {
        task.secondNumber = rand() % 3 + 2;   // Exponent 2 bis 4
    }

    task.solution = solve(task.firstNumber, task.secondNumber, task.operation);
    return task;
}