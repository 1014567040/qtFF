#include "mainwindow.h"

#include <QApplication>
extern "C"{
    #include<libavdevice/avdevice.h>//设备
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
