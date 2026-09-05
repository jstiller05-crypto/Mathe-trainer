// task_generator.cpp
#include "task_generator.h"
#include "arithmetic_unit.h"
#include <QDebug>

Task generateTask(DifficultyLevel level, const QVector<QPair<QString, QString>> &activeSelections, TaskMode mode)
{
    QStringList arithmeticSubs;
    for (const auto &selection : activeSelections) {
        if (selection.first == "Arithmetik") arithmeticSubs.append(selection.second);
    }

    // Eine leere Liste ist jetzt ein gueltiger, gewollter Zustand (siehe SidebarMenu -
    // alle Haekchen abgewaehlt bedeutet "alle verfuegbaren Typen"), keine Fehlerlage
    // mehr - arithmetic_unit.cpp behandelt das selbst (generateArithmeticTask()).
    qDebug() << "[TaskGenerator] Arithmetik-Unterkategorien aus Auswahl:" << arithmeticSubs;
    return generateArithmeticTask(level, arithmeticSubs, mode);
}