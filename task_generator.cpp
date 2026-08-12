#include "task_generator.h"
#include <cstdlib>
#include <cmath>
#include <QString>

// --- Hilfsfunktion: bestimmt anhand des Levels, welches Thema drankommt ---
Topic pickTopicForLevel(DifficultyLevel level)
{
    if (level <= 10) return Topic::BasicArithmetic;
    if (level <= 20) return (rand() % 2 == 0) ? Topic::BasicArithmetic : Topic::UnitConversion;
    if (level <= 30) return Topic::OrderOfOperations;
    if (level <= 40) return Topic::NegativeNumbers;
    if (level <= 50) return (rand() % 2 == 0) ? Topic::Percentage : Topic::Interest;
    if (level <= 60) return Topic::Powers;
    if (level <= 70) return Topic::Roots;
    // ab 71: Mischung aus allem bisher Gelernten (typisch für "Abschluss"-Niveau)
    Topic pool[] = { Topic::Percentage, Topic::Powers, Topic::Roots, Topic::NegativeNumbers };
    return pool[rand() % 4];
}

// --- Ein Generator PRO Thema - jeder erzeugt Text + Lösung selbst ---

Task generateBasicArithmetic(DifficultyLevel level)
{
    int maxNumber = 10 + level * 10;   // Kl.3 (Level ~5) -> ~60, Kl.4 (Level ~15) -> ~160
    int first = rand() % maxNumber + 1;
    int second = rand() % maxNumber + 1;
    bool isAddition = (rand() % 2 == 0);

    Task task;
    task.topic = Topic::BasicArithmetic;
    task.solution = isAddition ? (first + second) : (first - second);
    task.questionText = QString("%1 %2 %3 = ?").arg(first).arg(isAddition ? "+" : "-").arg(second);
    return task;
}

Task generateOrderOfOperations(DifficultyLevel level)
{
    // z.B. 3 + 4 * 2 = ? -> testet "Punkt vor Strich"
    int a = rand() % 10 + 1;
    int b = rand() % 10 + 1;
    int c = rand() % 10 + 1;

    Task task;
    task.topic = Topic::OrderOfOperations;
    task.solution = a + (b * c);   // Punkt-vor-Strich fest einprogrammiert
    task.questionText = QString("%1 + %2 × %3 = ?").arg(a).arg(b).arg(c);
    return task;
}

Task generateNegativeNumbers(DifficultyLevel level)
{
    int a = rand() % 20 - 10;   // Bereich -10 bis +10
    int b = rand() % 20 - 10;

    Task task;
    task.topic = Topic::NegativeNumbers;
    task.solution = a + b;
    task.questionText = QString("(%1) + (%2) = ?").arg(a).arg(b);
    return task;
}

Task generatePercentage(DifficultyLevel level)
{
    int percent = (rand() % 10 + 1) * 10;   // 10, 20, ..., 100
    int base = (rand() % 20 + 1) * 10;       // 10, 20, ..., 200

    Task task;
    task.topic = Topic::Percentage;
    task.solution = (percent * base) / 100;
    task.questionText = QString("%1% von %2 = ?").arg(percent).arg(base);
    return task;
}

Task generatePowers(DifficultyLevel level)
{
    int base = rand() % 10 + 2;
    int exponent = rand() % 3 + 2;   // 2 bis 4

    Task task;
    task.topic = Topic::Powers;
    task.solution = static_cast<int>(std::pow(base, exponent));
    task.questionText = QString("%1^%2 = ?").arg(base).arg(exponent);
    return task;
}

// --- Die Haupt-Funktion: wählt Thema, ruft passenden Generator auf ---
Task generateTask(DifficultyLevel level)
{
    Topic topic = pickTopicForLevel(level);

    switch (topic) {
    case Topic::BasicArithmetic:    return generateBasicArithmetic(level);
    case Topic::OrderOfOperations:  return generateOrderOfOperations(level);
    case Topic::NegativeNumbers:    return generateNegativeNumbers(level);
    case Topic::Percentage:         return generatePercentage(level);
    case Topic::Powers:             return generatePowers(level);
    default:
        // Noch nicht implementierte Themen (Interest, Roots, UnitConversion)
        // fallen vorerst auf BasicArithmetic zurück, bis sie ausgebaut sind
        return generateBasicArithmetic(level);
    }
}