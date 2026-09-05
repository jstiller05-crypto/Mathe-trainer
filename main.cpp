#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QStyleHints>
#include <QGuiApplication>
#include <QIcon>
#include <QFont>
#include <QDebug>
#include <cstdlib>
#include <ctime>

// Eigene Hilfsfunktion: laedt eine Schriftart, gibt den Familiennamen zurueck
// (oder einen Ersatzwert, falls es fehlschlaegt) - faengt Fehler ab statt abzustuerzen.
QString loadAppFont(const QString &path, const QString &fallback = "Arial")
{
    int fontId = QFontDatabase::addApplicationFont(path);

    if (fontId == -1) {
        qWarning() << "Font could not be loaded from:" << path << "- using fallback:" << fallback;
        return fallback;
    }

    QStringList families = QFontDatabase::applicationFontFamilies(fontId);
    if (families.isEmpty()) {
        qWarning() << "Font loaded but contains no font families:" << path;
        return fallback;
    }

    return families.at(0);
}

// Eigene Hilfsfunktion: laedt ein Icon und prueft, ob es wirklich geklappt hat.
bool loadAppIcon(QApplication &app, MainWindow &window, const QString &path)
{
    QIcon icon(path);
    if (icon.isNull()) {
        qWarning() << "Icon could not be loaded from:" << path;
        return false;
    }
    app.setWindowIcon(icon);
    window.setWindowIcon(icon);
    return true;
}

int main(int argc, char *argv[])
{
    // Ohne srand() startet rand() (genutzt in allen Generatoren) bei jedem Programmstart
    // mit demselben Standard-Seed (1) - die App wuerde also bei jedem Start exakt dieselbe
    // Aufgabenfolge zeigen. Einmaliges Seeden mit der aktuellen Uhrzeit reicht hier aus.
    // (Die sauberere Umstellung auf QRandomGenerator in allen Generatoren ist ein
    // separater, spaeterer Schritt - hier bewusst NICHT mit erledigt.)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    QApplication app(argc, argv);

    // --- Schriftarten laden: Text-Schrift und Zahlen-Schrift getrennt ---
    QString textFontFamily = loadAppFont(":/resources/fonts/IBMPlexSans-Regular.ttf", "Arial");
    QString numberFontFamily = loadAppFont(":/resources/fonts/MartelSans-Regular.ttf", textFontFamily);

    app.setFont(QFont(textFontFamily, 11));

    // --- Hauptfenster erzeugen, Zahlen-Schrift wird durchgereicht ---
    MainWindow window(numberFontFamily);

    // --- Icon laden (nach Fenster-Erzeugung, weil loadAppIcon das Fenster braucht) ---
    loadAppIcon(app, window, ":/resources/icons/app.ico");

    window.show();
    return QApplication::exec();
}