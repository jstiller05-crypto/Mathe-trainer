#include "sidebar_menu.h"
#include <QMouseEvent>
#include <QDebug>

struct CategoryDefinition {
    QString name;
    QStringList subcategories;
    bool enabled;
};

SidebarMenu::SidebarMenu(QWidget *parent)
    : QWidget(parent)
{
    resize(collapsedWidth, 400);

    layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(8, 20, 8, 20);

    mentalMathButton = new QPushButton("Σ Kopfrechnen", this);
    mentalMathButton->setObjectName("mentalMathButton");
    mentalMathButton->setCheckable(true);
    mentalMathButton->setChecked(true);
    mentalMathButton->installEventFilter(this);
    layout->addWidget(mentalMathButton);

    connect(mentalMathButton, &QPushButton::toggled, this, [this](bool checked) {
        emit mentalMathModeChanged(checked);
        qDebug() << "[Sidebar] Kopfrechnen-Modus:" << checked;
    });

    testGridButton = new QPushButton("🧪 Test: Raster", this);
    testGridButton->setObjectName("testGridButton");
    testGridButton->installEventFilter(this);
    layout->addWidget(testGridButton);

    connect(testGridButton, &QPushButton::clicked, this, [this]() {
        emit testWrittenGridRequested();
    });

    buildCategoryTree();

    layout->addStretch();

    settingsButton = new QPushButton("⚙", this);
    settingsButton->setObjectName("settingsButton");
    settingsButton->installEventFilter(this);
    connect(settingsButton, &QPushButton::clicked, this, [this]() {
        emit settingsClicked();
    });
    layout->addWidget(settingsButton);

    widthAnimation = new QPropertyAnimation(this, "barWidth", this);
    widthAnimation->setDuration(300);
    connect(widthAnimation, &QPropertyAnimation::finished, this, [this]() {
        if (!expanded) updateLabels();
    });

    collapseTimer = new QTimer(this);
    collapseTimer->setSingleShot(true);
    collapseTimer->setInterval(1000);
    connect(collapseTimer, &QTimer::timeout, this, [this]() {
        if (expanded) toggleExpanded();
    });

    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(R"(
        SidebarMenu {
            background-color: rgba(128, 128, 128, 76);
            border-right: 1px solid rgba(255, 255, 255, 30);
        }
        QPushButton {
            background-color: transparent;
            border: 1px solid transparent;
            font-size: 12pt;
            padding: 8px;
            border-radius: 6px;
            text-align: left;
            color: white;
        }
        QPushButton:hover {
            border: 1px solid #5B8DEF;
        }
        QPushButton:disabled {
            color: rgba(255, 255, 255, 90);
        }
        QPushButton#mentalMathButton:checked {
            border: 1px solid #5B8DEF;
            background-color: rgba(91, 141, 239, 40);
        }
        QPushButton#subCategoryButton:checked {
            border: 1px solid #5B8DEF;
            background-color: rgba(91, 141, 239, 40);
        }
    )");
}

void SidebarMenu::buildCategoryTree()
{
    QVector<CategoryDefinition> categories = {
        { "Arithmetik", { "Addition", "Subtraktion", "Multiplikation", "Division", "Prozentrechnung", "Potenz", "Wurzel", "Logarithmus", "Finanzen & Einheiten" }, true },
        { "Trigonometrie", { "Kopfrechenaufgaben", "Winkelberechnung", "Seitenberechnung", "Dreiecksberechnung", "Sinus-/Kosinussatz", "Einheitskreis" }, false },
        { "Geometrie", { "Kopfrechenaufgaben", "Volumenberechnung", "Flächeninhalt", "Mantel/Oberfläche", "Umfang", "Ähnlichkeit/Maßstab", "Koordinatengeometrie" }, false },
        { "Algebra", { "Lineare Funktionen", "Parabeln", "Exponentielle Funktionen", "Gleichungen lösen", "Ungleichungen", "Lineare Gleichungssysteme" }, false },
        { "Stochastik", { "Wahrscheinlichkeit", "Bedingte Wahrscheinlichkeit", "Kombinatorik", "Statistische Kennwerte" }, false },
        { "Analysis", { "Ableitungen", "Integrale" }, false }
    };

    for (const CategoryDefinition &def : categories) {
        CategoryBlock block;
        block.fullName = def.name;

        block.headerButton = new QPushButton(def.name, this);
        block.headerButton->setObjectName("categoryHeader");
        block.headerButton->installEventFilter(this);
        layout->addWidget(block.headerButton);

        block.subContainer = new QWidget(this);
        QVBoxLayout *subLayout = new QVBoxLayout(block.subContainer);
        subLayout->setContentsMargins(16, 0, 0, 0);
        subLayout->setSpacing(2);

        for (const QString &sub : def.subcategories) {
            QPushButton *subButton = new QPushButton(sub, block.subContainer);
            subButton->setObjectName("subCategoryButton");
            subButton->setEnabled(def.enabled);
            subButton->setCheckable(true);
            subButton->setProperty("subcategoryName", sub);   // NEU - fuer zuverlaessiges Wiedererkennen

            if (def.name == "Arithmetik" && sub == "Addition") {   // <- "Addition & Subtraktion" -> "Addition"
                subButton->setChecked(true);
            }

            QString categoryName = def.name;
            connect(subButton, &QPushButton::toggled, this, [this, categoryName, sub, subButton](bool checked) {
                QPair<QString, QString> key(categoryName, sub);

                if (!checked && activeSelections.size() == 1 && activeSelections.contains(key)) {
                    qDebug() << "[Sidebar] Letzte aktive Auswahl kann nicht abgeschaltet werden:" << key;
                    subButton->blockSignals(true);
                    subButton->setChecked(true);
                    subButton->blockSignals(false);
                    return;
                }

                if (checked) {
                    if (!activeSelections.contains(key)) activeSelections.append(key);
                } else {
                    activeSelections.removeAll(key);
                }
                emit activeSelectionsChanged(activeSelections);
                qDebug() << "[Sidebar] Aktive Auswahl:" << activeSelections;
            });

            subLayout->addWidget(subButton);
            block.subButtons.append(subButton);   // NEU - fuer setAvailableSubcategories() gebraucht

            if (def.name == "Arithmetik" && sub == "Addition & Subtraktion") {
                subButton->setChecked(true);
            }
        }

        block.subContainer->setVisible(false);
        layout->addWidget(block.subContainer);

        categoryBlocks.append(block);

        int blockIndex = categoryBlocks.size() - 1;
        connect(block.headerButton, &QPushButton::clicked, this, [this, blockIndex]() {
            CategoryBlock &b = categoryBlocks[blockIndex];
            b.subVisible = !b.subVisible;
            b.subContainer->setVisible(b.subVisible && expanded);
        });
    }
}

void SidebarMenu::setAvailableSubcategories(const QString &category, const QStringList &availableSubcategories)
{
    for (CategoryBlock &block : categoryBlocks) {
        if (block.fullName != category) continue;

        for (QPushButton *btn : block.subButtons) {
            QString subName = btn->property("subcategoryName").toString();
            bool isAvailable = availableSubcategories.contains(subName);

            btn->setVisible(isAvailable);

            if (!isAvailable && btn->isChecked()) {
                btn->setChecked(false);   // loest automatisch das toggled-Signal aus, entfernt es aus activeSelections
            }
        }
    }
}

int SidebarMenu::barWidth() const
{
    return width();
}

void SidebarMenu::setBarWidth(int newWidth)
{
    resize(newWidth, height());
}

void SidebarMenu::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (expanded) collapseTimer->start();
}

void SidebarMenu::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    collapseTimer->stop();
}

void SidebarMenu::toggleExpanded()
{
    expanded = !expanded;
    int target = expanded ? expandedWidth : collapsedWidth;

    if (expanded) {
        updateLabels();
    }

    widthAnimation->stop();
    widthAnimation->setStartValue(barWidth());
    widthAnimation->setEndValue(target);
    widthAnimation->start();
}

void SidebarMenu::updateLabels()
{
    for (CategoryBlock &block : categoryBlocks) {
        block.headerButton->setText(expanded ? block.fullName : block.fullName.left(2));
        if (!expanded) {
            block.subVisible = false;
            block.subContainer->setVisible(false);
        }
    }
    settingsButton->setText(expanded ? "Einstellungen" : "⚙");
}

bool SidebarMenu::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress && !expanded) {
        toggleExpanded();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}