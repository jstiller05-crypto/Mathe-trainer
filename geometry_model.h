#ifndef GEOMETRY_MODEL_H
#define GEOMETRY_MODEL_H

#include <QVector>
#include <QString>
#include <QPointF>

// Eine Kante verbindet zwei Punkte ueber ihren INDEX in GeometryModel::points (nicht
// per Kopie oder Zeiger) - so bleibt das Modell konsistent, selbst wenn sich ein
// Punkt spaeter (z.B. beim Skalieren fuers Zeichnen) verschiebt: es gibt dann nur
// EINE Stelle (den Punkt in der Liste), die sich aendert, alle Kanten "folgen"
// automatisch mit.
struct GeometryEdge {
    int fromPointIndex;
    int toPointIndex;
};

// Eine Beschriftung an einer Position im Modell-Koordinatensystem - entweder ein
// FESTER Text (ein GEGEBENER Wert der Aufgabe, z.B. "5 cm") ODER ein Verweis auf ein
// GESUCHTES Antwortfeld (answerIndex in Task::answers, siehe task.h). Ein Label mit
// answerIndex >= 0 markiert also die Stelle, an der die spaetere Zeichnung z.B. ein
// Eingabefeld oder ein Fragezeichen statt eines festen Textes anzeigen muss.
// answerIndex = -1 bedeutet "kein Verweis, es gilt der feste text" - bewusst ein
// einfacher int statt std::optional<int>, das haelt dieses kleine Struct simpel und
// passt zum Rest des Projekts (kein <optional> ausser bei Task::geometryModel selbst,
// wo der Unterschied "Geometrie-Aufgabe ja/nein" eine echte Bedeutung hat).
struct GeometryLabel {
    QPointF position;
    QString text;            // fester Anzeige-Text, NUR gueltig wenn answerIndex < 0
    int answerIndex = -1;    // >= 0: Index in Task::answers - GESUCHTE Groesse statt festem Text
};

// Rein geometrisches Modell einer Aufgabe (aktuell nur Rechteck, spaeter Dreieck/
// Kreis/Koerper) - bewusst NICHT massstabsgetreu: die Koordinaten sind ein neutrales
// Layout NUR fuer die spaetere Anordnung beim Zeichnen (z.B. landet ein 3x12-
// Rechteck hier trotzdem als handliches, nicht extrem laenglich verzerrtes Viereck,
// damit es auf dem Bildschirm vernuenftig aussieht). Die ECHTEN Masse der Aufgabe
// stehen ausschliesslich in den GeometryLabel-Texten bzw. in Task::answers, NIEMALS
// in den Punktkoordinaten selbst - wer die Koordinaten als "das Rechteck ist 3x12"
// interpretiert, macht einen Fehler.
struct GeometryModel {
    QVector<QPointF> points;
    QVector<GeometryEdge> edges;
    QVector<GeometryLabel> labels;
};

#endif
