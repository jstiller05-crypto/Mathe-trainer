#include "arithmetic_unit.h"
#include "addition_subtraction_generator.h"
#include "percent_mult_div_generator.h"
#include "root_power_log_generator.h"
#include "finance_generator.h"
#include "units_generator.h"
#include <cstdlib>
#include <QDebug>

Task generateArithmeticTask(DifficultyLevel level)
{
    // 0-4: einer der 5 Generatoren erzeugt eine EIGENSTAENDIGE Aufgabe
    // 5: Verschmelzung Wurzel/Potenz/Log + Addition/Subtraktion (z.B. "4² + 8")
    int choice = rand() % 6;

    qDebug() << "[ArithmeticUnit] Auswahl:" << choice << "bei Level" << level;

    switch (choice) {
    case 0: return generateAdditionSubtractionTask(level);
    case 1: return generatePercentMultDivTask(level);
    case 2: return generateRootPowerLogTask(level);
    case 3: return generateFinanceTask(level);
    case 4: return generateUnitsTask(level);
    default: {
        TaskFragment fragment = generateRootPowerLogFragment(level);
        Task combined = combineWithAdditionSubtraction(fragment, level);
        qDebug() << "[ArithmeticUnit] Verschmolzen:" << combined.promptText;
        return combined;
    }
    }
}