#include "lupoReadout.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    lupoReadout w;
    w.show();
    return a.exec();
}
