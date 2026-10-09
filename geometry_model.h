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

// Eine Beschriftung an einer Position im Modell-Koordinatensystem. answerIndex
// entscheidet, WIE die Zeichnung sie darstellt: answerIndex = -1 bedeutet "fester,
// GEGEBENER Wert der Aufgabe" (text wird direkt an der position gezeichnet, z.B.
// "5 cm"); answerIndex >= 0 ist ein Verweis auf ein GESUCHTES Antwortfeld (Index in
// Task::answers, siehe task.h) - hier wird an der position stattdessen ein echtes
// Eingabefeld eingebettet. text bleibt AUCH in diesem Fall gueltig: es dient dann als
// kurze Erklaerung DAVOR (z.B. "Fläche ="), damit klar ist, WAS dort gesucht ist -
// dementsprechend darf text hier auch leer bleiben, wenn keine Erklaerung noetig ist
// (z.B. wenn die gesuchte Kante selbsterklaerend neben einer bereits gegebenen liegt).
// Bewusst ein einfacher int statt std::optional<int> fuer answerIndex, das haelt dieses
// kleine Struct simpel und passt zum Rest des Projekts (kein <optional> ausser bei
// Task::geometryModel selbst, wo der Unterschied "Geometrie-Aufgabe ja/nein" eine echte
// Bedeutung hat).
struct GeometryLabel {
    QPointF position;
    QString text;            // Kurzbeschriftung - fester Wert (answerIndex < 0) ODER Erklaerung vor einem Eingabefeld (answerIndex >= 0)
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
