#include "settings_view.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

SettingsView::SettingsView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 40, 60, 40);
    mainLayout->setSpacing(20);

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

    mainLayout->addStretch();
}