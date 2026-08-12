#ifndef TASK_H
#define TASK_H
#include <QString>
#include "topic.h"
#include "operation.h"

// Ein "struct" - nur ein Behälter für zusammengehörige Daten, keine Logik
struct Task {
    Topic topic;
    QString questionText;   // z.B. "23 + 47 = ?" oder "15 % von 200 = ?"
    int solution;
};

#endif // TASK_H
