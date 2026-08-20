#include "sidebar_menu.h"
#include <QMouseEvent>

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
    )");
}

void SidebarMenu::buildCategoryTree()
{
    // Baumstruktur - nur "Arithmetik" ist aktuell funktional (enabled = true),
    // der Rest steht schon als Geruest da (Roadmap: [[mathe-lern-app-roadmap]])
    QVector<CategoryDefinition> categories = {
        { "Arithmetik", { "Kopfrechenaufgaben", "Finanzrechnung", "Bruch-/Prozentrechnung", "Wurzel/Potenz/Logarithmus", "Größen-/Einheiten-Umrechnung", "Rundung/Überschlag" }, true },
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
            QString categoryName = def.name;
            connect(subButton, &QPushButton::clicked, this, [this, categoryName, sub]() {
                emit categorySelected(categoryName, sub);
            });
            subLayout->addWidget(subButton);
        }

        block.subContainer->setVisible(false);
        layout->addWidget(block.subContainer);

        categoryBlocks.append(block);

        // Index NACH dem Anhaengen holen, damit der Zugriff in der Lambda stabil bleibt
        int blockIndex = categoryBlocks.size() - 1;
        connect(block.headerButton, &QPushButton::clicked, this, [this, blockIndex]() {
            CategoryBlock &b = categoryBlocks[blockIndex];
            b.subVisible = !b.subVisible;
            b.subContainer->setVisible(b.subVisible && expanded);
        });
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