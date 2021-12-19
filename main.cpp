#include "gobangwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GoBangWidget w;
    w.show();
    return a.exec();
}
