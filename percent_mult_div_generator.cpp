#include "percent_mult_div_generator.h"
#include <cstdlib>
#include <QDebug>

static int maxFactorForLevel(DifficultyLevel level)
{
    return 9 + level / 2;   // kleines 1x1 -> grosses 1x1 -> darueber hinaus
}

Task generatePercentMultDivTask(DifficultyLevel level)
{
    int maxFactor = maxFactorForLevel(level);
    int choice = rand() % 3;   // 0=Multiplikation, 1=Division, 2=Prozent

    Task task;
    task.autoAdvance = true;

    if (choice == 0) {
        int a = rand() % maxFactor + 1;
        int b = rand() % maxFactor + 1;
        task.ruleName = "Multiplication";
        task.promptText = QString("%1 × %2 =").arg(a).arg(b);
        task.answers.append({ "", static_cast<double>(a * b) });
    } else if (choice == 1) {
        // WICHTIG: erst Divisor + Ergebnis waehlen, DANN den Dividenden daraus
        // berechnen - so geht die Division IMMER glatt auf (kein Rest)
        int divisor = rand() % 10 + 2;
        int quotient = rand() % maxFactor + 1;
        int dividend = divisor * quotient;
        task.ruleName = "Division";
        task.promptText = QString("%1 ÷ %2 =").arg(dividend).arg(divisor);
        task.answers.append({ "", static_cast<double>(quotient) });
    } else {
        int percentSteps[] = {5, 10, 20, 25, 50, 75};
        int percent = percentSteps[rand() % 6];
        int base = (rand() % 20 + 1) * 10;   // glatte Zehnerzahl
        task.ruleName = "Percentage";
        task.promptText = QString("%1% von %2 =").arg(percent).arg(base);
        task.answers.append({ "", (percent * base) / 100.0 });
    }

    qDebug() << "[PercentMultDiv] Standalone:" << task.promptText;
    return task;
}

TaskFragment generatePercentMultDivFragment(DifficultyLevel level)
{
    Q_UNUSED(level);
    int a = rand() % 10 + 1;
    int b = rand() % 10 + 1;

    TaskFragment fragment;
    fragment.value = a * b;
    fragment.display = QString("%1 × %2").arg(a).arg(b);
    return fragment;
}