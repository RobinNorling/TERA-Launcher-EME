#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath(".");
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setFixedSize(w.width(), w.height());
    w.show();

    return a.exec();
}
