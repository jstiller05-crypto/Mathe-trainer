#include "task_generator.h"
#include <cstdlib>   // für rand()
#include <cmath>
#include <QDebug>

// Kleine Hilfsfunktion: rechnet EINE gegebene Aufgabe aus.
// Das ist der einzige Ort im ganzen Programm, an dem tatsächlich gerechnet wird.
int solve(int first, int second, Operation operation)
{
    switch (operation) {
    case Operation::Addition:       return first + second;
    case Operation::Subtraction:    return first - second;
    case Operation::Multiplication: return first * second;
    case Operation::Power:          return static_cast<int>(std::pow(first, second));
    }
    return 0;   // sollte nie erreicht werden, Absicherung für den Compiler
}

// Legt fest, WELCHE Operationen und WELCHER Zahlenbereich bei welcher Stufe erlaubt sind
Task generateTask(Difficulty difficulty)
{
    Task task;

    int maxNumber;
    Operation allowedOperations[4];
    int operationCount;

    switch (difficulty) {
    case Difficulty::Beginner:
        maxNumber = 10;
        allowedOperations[0] = Operation::Addition;
        allowedOperations[1] = Operation::Subtraction;
        operationCount = 2;
        break;
    case Difficulty::Intermediate:
        maxNumber = 20;
        allowedOperations[0] = Operation::Addition;
        allowedOperations[1] = Operation::Subtraction;
        allowedOperations[2] = Operation::Multiplication;
        operationCount = 3;
        break;
    case Difficulty::Advanced:
        maxNumber = 12;
        allowedOperations[0] = Operation::Multiplication;
        allowedOperations[1] = Operation::Power;
        operationCount = 2;
        break;
    }

    task.firstNumber = rand() % maxNumber + 1;
    task.secondNumber = rand() % maxNumber + 1;

    // Bei Potenzen: Exponent klein halten, sonst kommen absurd große Zahlen raus
    if (allowedOperations[rand() % operationCount] == Operation::Power) {
        task.secondNumber = rand() % 3 + 2;   // Exponent zwischen 2 und 4
    }

    task.operation = allowedOperations[rand() % operationCount];
    task.solution = solve(task.firstNumber, task.secondNumber, task.operation);

    // ... Berechnung wie oben ...
    qDebug() << "Task generated - Difficulty:" << static_cast<int>(difficulty)
             << "| " << task.firstNumber << "op" << static_cast<int>(task.operation)
             << task.secondNumber << "=" << task.solution;

    return task;
}