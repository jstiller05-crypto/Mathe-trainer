#include "finance_generator.h"
#include <cstdlib>
#include <QDebug>

Task generateFinanceTask(DifficultyLevel level)
{
    Q_UNUSED(level);   // TODO: eigene Level-Skalierung fuer Finanzrechnung noch offen

    int capital = (rand() % 20 + 1) * 100;
    int interestSteps[] = {1, 2, 3, 4, 5, 10};
    int interestPercent = interestSteps[rand() % 6];

    Task task;
    task.ruleName = "Finance";
    task.promptText = QString("Kapital: %1€, Zinssatz: %2% - Zinsen nach 1 Jahr =")
                          .arg(capital).arg(interestPercent);
    task.answers.append({ "", (capital * interestPercent) / 100.0 });
    task.autoAdvance = true;

    qDebug() << "[Finance] Standalone:" << task.promptText;
    return task;
}