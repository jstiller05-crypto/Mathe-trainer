#include "operation.h"

QString operationToSymbol(Operation operation)
{
    switch (operation) {
    case Operation::Addition:       return "+";
    case Operation::Subtraction:    return "-";
    case Operation::Multiplication: return "×";
    case Operation::Power:          return "^";
    }
    return "?";
}