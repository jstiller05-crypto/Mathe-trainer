#include "task_generator.h"
#include "arithmetic_unit.h"
#include <QDebug>

// Die Zentrale (Sternsystem-Nabe): fragt aktuell nur "Arithmetik" - sobald
// weitere Units existieren, entscheidet diese Funktion je nach gewaehlter
// Kategorie, welche Unit gefragt wird.
Task generateTask(DifficultyLevel level)
{
    qDebug() << "[TaskGenerator] Anfrage fuer Level" << level << "- aktuell nur Arithmetik verfuegbar";
    return generateArithmeticTask(level);
}