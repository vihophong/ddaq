#include "mainwindow.h"
#include <QApplication>
#include "TQtWidget.h"
#include "TGraph.h"
#include "TCanvas.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    

    MainWindow w;
    w.show();

    return a.exec();
}
