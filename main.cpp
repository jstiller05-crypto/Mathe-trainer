#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QStyleHints>
#include <QGuiApplication>
#include <QIcon>
#include <QFont>
#include <QFile>
#include <QDebug>

// Eigene Hilfsfunktion: lädt ein Icon und prüft, ob es wirklich geklappt hat.
// Gibt true/false zurück, damit main() weiß, ob es geklappt hat.
bool loadAppIcon(QApplication &app, MainWindow &window, const QString &path)
{
    qDebug() << "Icon exists:" << QFile::exists(":/resources/icons/app.ico");
    qDebug() << "Font exists:" << QFile::exists(":/resources/fonts/Inter_24pt-Regular.ttf");
    QIcon icon(path);
    if (icon.isNull()) {
        qWarning() << "Icon could not be loaded from:" << path;
        return false;
    }
    app.setWindowIcon(icon);
    window.setWindowIcon(icon);
    return true;
}

// Eigene Hilfsfunktion: lädt eine Schriftart, gibt den Familiennamen zurück
// (oder einen Ersatzwert, falls es fehlschlägt)
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;   // Icon-Funktion braucht ein bereits existierendes MainWindow-Objekt

    loadAppIcon(app, window, ":/resources/icons/app.ico");

    QString fontFamily = loadAppFont(":/resources/fonts/Inter_24pt-Regular.ttf");
    app.setFont(QFont(fontFamily, 11));

    window.show();
    return QApplication::exec();
}