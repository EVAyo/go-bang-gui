#include "gobangwidget.h"
//#include "ChessEngine.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GoBangWidget w;
    w.show();

//    ChessEngine::beforeStart();
//    ChessEngine::setLevel(3);
//    ChessEngine::reset(0);
//    ChessEngine::nextStep(0, 1);
    return a.exec();
}
