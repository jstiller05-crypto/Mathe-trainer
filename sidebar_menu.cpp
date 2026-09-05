#include "sidebar_menu.h"
#include <QMouseEvent>
#include <QDebug>

// implemented = false -> Button ist sichtbar, aber dauerhaft deaktiviert (Tooltip
// "Generator folgt noch"). So kann man schon sehen, was geplant ist, ohne dass ein
// Klick still auf Addition zurueckfaellt (siehe standaloneForSubcategory() in
// arithmetic_unit.cpp), weil es fuer diese Unterkategorie noch keinen Generator gibt.
struct SubcategoryDefinition {
    QString name;
    bool implemented;
};

struct CategoryDefinition {
    QString name;
    QVector<SubcategoryDefinition> subcategories;
    bool enabled;   // ganze Kategorie (Trigonometrie etc. ist komplett noch nicht gebaut)
};

SidebarMenu::SidebarMenu(QWidget *parent)
    : QWidget(parent)
{
    resize(collapsedWidth, 400);

    layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(8, 20, 8, 20);

    // Kopfrechnen/Taschenrechner/Schwere Aufgabe und Aufgabenblatt sind in die
    // Einstellungen umgezogen (siehe settings_view.cpp) - die Sidebar zeigt nur noch
    // Aufgaben-Kategorien, analog zur Klassenstufe, die schon vorher dort war.
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

    // Sammelpuffer fuer activeSelectionsChanged: mehrere Klicks kurz hintereinander
    // (z.B. schnelles Durchklicken mehrerer Unterkategorien) sollen nur EINE
    // Neugenerierung ausloesen statt bei jedem einzelnen Klick sofort eine neue
    // Aufgabe zu bauen - das hat vorher spuerbar geruckelt.
    selectionDebounceTimer = new QTimer(this);
    selectionDebounceTimer->setSingleShot(true);
    selectionDebounceTimer->setInterval(200);
    connect(selectionDebounceTimer, &QTimer::timeout, this, [this]() {
        qDebug() << "[Sidebar] activeSelectionsChanged (Sammelpuffer abgelaufen):" << activeSelections;
        emit activeSelectionsChanged(activeSelections);
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
        QPushButton#subCategoryButton:checked {
            border: 1px solid #5B8DEF;
            background-color: rgba(91, 141, 239, 40);
        }
    )");
}

void SidebarMenu::buildCategoryTree()
{
    QVector<CategoryDefinition> categories = {
        { "Arithmetik", {
              { "Addition", true }, { "Subtraktion", true }, { "Multiplikation", true }, { "Division", true },
              { "Prozentrechnung", true }, { "Potenz", true }, { "Wurzel", true }, { "Logarithmus", true },
              { "Größen & Einheiten", true },
              // Platzhalter (Punkt 5) - Generatoren fehlen noch, Buttons bleiben sichtbar+deaktiviert.
              { "Bruchrechnung", false }, { "Dezimalzahlen", false }, { "Klammern & Terme", false },
              { "Negative Zahlen", false }, { "Teilbarkeit (ggT/kgV)", false }, { "Finanzrechnung", false }
          }, true },
        { "Trigonometrie", { {"Kopfrechenaufgaben",false}, {"Winkelberechnung",false}, {"Seitenberechnung",false}, {"Dreiecksberechnung",false}, {"Sinus-/Kosinussatz",false}, {"Einheitskreis",false} }, false },
        { "Geometrie", { {"Kopfrechenaufgaben",false}, {"Volumenberechnung",false}, {"Flächeninhalt",false}, {"Mantel/Oberfläche",false}, {"Umfang",false}, {"Ähnlichkeit/Maßstab",false}, {"Koordinatengeometrie",false} }, false },
        { "Algebra", { {"Lineare Funktionen",false}, {"Parabeln",false}, {"Exponentielle Funktionen",false}, {"Gleichungen lösen",false}, {"Ungleichungen",false}, {"Lineare Gleichungssysteme",false} }, false },
        { "Stochastik", { {"Wahrscheinlichkeit",false}, {"Bedingte Wahrscheinlichkeit",false}, {"Kombinatorik",false}, {"Statistische Kennwerte",false} }, false },
        { "Analysis", { {"Ableitungen",false}, {"Integrale",false} }, false }
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

        for (const SubcategoryDefinition &subcat : def.subcategories) {
            QPushButton *subButton = new QPushButton(subcat.name, block.subContainer);
            subButton->setObjectName("subCategoryButton");
            subButton->setEnabled(def.enabled && subcat.implemented);
            subButton->setCheckable(true);
            subButton->setProperty("subcategoryName", subcat.name);   // NEU - fuer zuverlaessiges Wiedererkennen
            subButton->setProperty("implemented", subcat.implemented);   // NEU - fuer setAvailableSubcategories()

            if (!subcat.implemented) {
                subButton->setToolTip(tr("Generator folgt noch"));
                subLayout->addWidget(subButton);
                block.subButtons.append(subButton);
                continue;   // Platzhalter nimmt nicht an Auswahl/activeSelections teil
            }

            if (def.name == "Arithmetik" && subcat.name == "Addition") {
                subButton->setChecked(true);
            }

            QString categoryName = def.name;
            QString subName = subcat.name;
            connect(subButton, &QPushButton::toggled, this, [this, categoryName, subName](bool checked) {
                QPair<QString, QString> key(categoryName, subName);

                if (checked) {
                    if (!activeSelections.contains(key)) activeSelections.append(key);
                } else {
                    activeSelections.removeAll(key);
                }
                qDebug() << "[Sidebar] Aktive Auswahl geaendert:" << activeSelections;

                if (updatingSelections) return;   // setAvailableSubcategories() emittiert selbst am Ende

                // Sammelpuffer statt Sofort-Emit (Punkt 2b) - siehe selectionDebounceTimer weiter oben.
                selectionDebounceTimer->start();
            });

            subLayout->addWidget(subButton);
            block.subButtons.append(subButton);   // NEU - fuer setAvailableSubcategories() gebraucht
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
    // Waehrend dieser Schleife unterdrueckt updatingSelections das Sofort-/Sammelpuffer-
    // Signal aus dem toggled-Lambda oben (Punkt 2b) - vorher hat setChecked(false) hier
    // bei JEDEM betroffenen Button einzeln (und verschachtelt) eine Neugenerierung
    // ausgeloest. Stattdessen wird ganz am Ende genau EINMAL emittiert.
    updatingSelections = true;

    for (CategoryBlock &block : categoryBlocks) {
        if (block.fullName != category) continue;

        for (QPushButton *btn : block.subButtons) {
            // Platzhalter (Punkt 5) tauchen nie in arithmeticAvailableSubcategories() auf
            // und wuerden hier sonst dauerhaft ausgeblendet - sie bleiben unabhaengig von
            // der Klassenstufe sichtbar und deaktiviert.
            if (!btn->property("implemented").toBool()) continue;

            QString subName = btn->property("subcategoryName").toString();
            bool isAvailable = availableSubcategories.contains(subName);

            btn->setVisible(isAvailable);

            if (!isAvailable && btn->isChecked()) {
                btn->setChecked(false);   // loest toggled() aus, aktualisiert activeSelections (aber ohne Signal, s.o.)
            }
        }
    }

    updatingSelections = false;
    selectionDebounceTimer->stop();   // ein noch laufender Sammelpuffer von vorherigen Klicks ist jetzt ueberholt
    qDebug() << "[Sidebar] activeSelectionsChanged (setAvailableSubcategories, einmalig):" << activeSelections;
    emit activeSelectionsChanged(activeSelections);
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