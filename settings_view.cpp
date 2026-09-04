#include "settings_view.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>

SettingsView::SettingsView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 40, 60, 40);
    mainLayout->setSpacing(20);

    // Checked-Hervorhebung fuer die Schwierigkeits-Buttons - das globale QPushButton-
    // Stylesheet (siehe MainWindow-Konstruktor) kennt keinen :checked-Zustand.
    setStyleSheet(R"(
        QPushButton#taskModeButton:checked {
            border: 2px solid #5B8DEF;
            background-color: rgba(91, 141, 239, 60);
        }
    )");

    QLabel *title = new QLabel("Einstellungen", this);
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    QLabel *classLabel = new QLabel("Klassenstufe", this);
    classLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(classLabel);

    QGridLayout *grid = new QGridLayout();
    int row = 0, col = 0;
    for (int schoolClass = 3; schoolClass <= 10; ++schoolClass) {
        QPushButton *button = new QPushButton(QString("Klasse %1").arg(schoolClass), this);
        connect(button, &QPushButton::clicked, this, [this, schoolClass]() {
            emit classSelected(schoolClass);
        });
        grid->addWidget(button, row, col);
        col++;
        if (col >= 4) { col = 0; row++; }
    }
    mainLayout->addLayout(grid);

    // --- Kategorie "Schwierigkeit": Kopfrechnen / Taschenrechner / Schwere Aufgabe -
    // frueher lose Buttons in der Sidebar, jetzt hier zusammengefasst (analog zur
    // Klassenstufe, die aus demselben Grund schon vorher hierher gewandert ist).
    QLabel *difficultyLabel = new QLabel("Schwierigkeit", this);
    difficultyLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(difficultyLabel);

    QHBoxLayout *difficultyRow = new QHBoxLayout();

    QPushButton *mentalMathButton = new QPushButton("Σ Kopfrechnen", this);
    mentalMathButton->setObjectName("taskModeButton");
    mentalMathButton->setCheckable(true);
    mentalMathButton->setChecked(true);
    difficultyRow->addWidget(mentalMathButton);

    QPushButton *calculatorButton = new QPushButton("🖩 mit Taschenrechner", this);
    calculatorButton->setObjectName("taskModeButton");
    calculatorButton->setCheckable(true);
    difficultyRow->addWidget(calculatorButton);

    QPushButton *hardTaskButton = new QPushButton("⚡ Schwere Aufgabe", this);
    hardTaskButton->setObjectName("taskModeButton");
    hardTaskButton->setCheckable(true);
    difficultyRow->addWidget(hardTaskButton);

    mainLayout->addLayout(difficultyRow);

    // Gegenseitig ausschliessend (wie Radiobuttons) - es ist immer genau EIN Modus aktiv.
    QButtonGroup *taskModeGroup = new QButtonGroup(this);
    taskModeGroup->setExclusive(true);
    taskModeGroup->addButton(mentalMathButton);
    taskModeGroup->addButton(calculatorButton);
    taskModeGroup->addButton(hardTaskButton);

    connect(mentalMathButton, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) emit taskModeChanged(TaskMode::MentalMath);
    });
    connect(calculatorButton, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) emit taskModeChanged(TaskMode::Calculator);
    });
    connect(hardTaskButton, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) emit taskModeChanged(TaskMode::Hard);
    });

    // --- Ansicht: Aufgabenblatt - frueher loser Button in der Sidebar, jetzt ebenfalls
    // hier (ist wie Klassenstufe/Schwierigkeit eine Einstellung, keine Aufgaben-Kategorie).
    QLabel *viewLabel = new QLabel("Ansicht", this);
    viewLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(viewLabel);

    QPushButton *worksheetButton = new QPushButton("📄 Aufgabenblatt", this);
    worksheetButton->setObjectName("taskModeButton");   // gleiche Checked-Hervorhebung wiederverwenden
    worksheetButton->setCheckable(true);
    mainLayout->addWidget(worksheetButton);
    connect(worksheetButton, &QPushButton::toggled, this, [this](bool checked) {
        emit worksheetModeChanged(checked);
    });

    mainLayout->addStretch();
}