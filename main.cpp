#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv); // "a" -> "app", eindeutiger
    MainWindow window;               // "w" -> "window"
    window.show();
    return QApplication::exec();
}
