#ifndef TASK_H
#define TASK_H
#include "operation.h"

// Ein "struct" - nur ein Behälter für zusammengehörige Daten, keine Logik
struct Task {
    int firstNumber;
    int secondNumber;
    Operation operation;  // '+', '-' oder '*'
    int solution;
};

#endif // TASK_H
