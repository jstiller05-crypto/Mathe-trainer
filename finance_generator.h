#ifndef FINANCE_GENERATOR_H
#define FINANCE_GENERATOR_H

#include "task.h"
#include "difficulty.h"

// Finanzrechnung nutzt inhaltlich Prozent-Logik, hat aber eigene Begriffe
// (Kapital, Zinssatz) - deshalb eigene Datei statt Wiederverwendung.
Task generateFinanceTask(DifficultyLevel level);

#endif