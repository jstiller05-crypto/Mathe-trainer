#ifndef OPERATION_H
#define OPERATION_H
#include <QString>

// "enum class" - eine feste, benannte Liste von Möglichkeiten.
// Das ist type-safe: du kannst nicht aus Versehen Operation und irgendeine
// andere Zahl vermischen (anders als bei rand() % 3, wo "1" ungefähr alles sein könnte)
enum class Operation {
    Addition,
    Subtraction,
    Multiplication,
    Power       // Platz für später, noch nicht implementiert
};


QString operationToSymbol(Operation operation);

#endif // OPERATION_H
