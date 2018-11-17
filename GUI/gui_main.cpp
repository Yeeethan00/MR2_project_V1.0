#include "mainwindow.h"
#include <QApplication>
#include "gui_main.h"

int gui_main()//(int argc, char *argv[])
{
    char *az[2];int d;
    az[1]=(char *)"";
    QApplication a((int&)d, az);
    MainWindow w;
    w.show();

    return a.exec();
}
