#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Saiyajin en la Nube");
    a.setApplicationDisplayName("Saiyajin en la Nube");
    a.setWindowIcon(QIcon(":/Imagenes/icono.png"));

    MainWindow w;
    w.setWindowTitle("Saiyajin en la Nube");
    w.show();

    return a.exec();
}
