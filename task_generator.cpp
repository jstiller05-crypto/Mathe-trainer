// task_generator.cpp
#include "task_generator.h"
#include "arithmetic_unit.h"
#include "geometry_unit.h"
#include <QDebug>

Task generateTask(DifficultyLevel level, const QVector<QPair<QString, QString>> &activeSelections, TaskMode mode)
{
    QStringList arithmeticSubs;
    QStringList geometrySubs;
    for (const auto &selection : activeSelections) {
        if (selection.first == "Arithmetik") arithmeticSubs.append(selection.second);
        if (selection.first == "Geometrie") geometrySubs.append(selection.second);
    }

    // "Geometrie" hat noch keine Sidebar-UI-Anbindung (siehe Aufgabenbeschreibung) -
    // activeSelections kann also aktuell in der laufenden App nie einen Geometrie-
    // Eintrag enthalten. Trotzdem schon hier verdrahtet, damit generateGeometryTask()
    // ueber diesen normalen Weg erreichbar ist, sobald die Sidebar-Integration kommt -
    // fuer diesen Schritt wird es stattdessen testweise direkt aufgerufen (siehe main.cpp).
    if (!geometrySubs.isEmpty()) {
        qDebug() << "[TaskGenerator] Geometrie-Unterkategorien aus Auswahl:" << geometrySubs;
        return generateGeometryTask(level, geometrySubs, mode);
    }

    // Eine leere Liste ist jetzt ein gueltiger, gewollter Zustand (siehe SidebarMenu -
    // alle Haekchen abgewaehlt bedeutet "alle verfuegbaren Typen"), keine Fehlerlage
    // mehr - arithmetic_unit.cpp behandelt das selbst (generateArithmeticTask()).
    qDebug() << "[TaskGenerator] Arithmetik-Unterkategorien aus Auswahl:" << arithmeticSubs;
    return generateArithmeticTask(level, arithmeticSubs, mode);
}