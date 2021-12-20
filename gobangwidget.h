#ifndef GOBANGWIDGET_H
#define GOBANGWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QPainter>
#include <QIcon>
#include <QMouseEvent>
#include "Game.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GoBangWidget; }
QT_END_NAMESPACE

class GoBangWidget : public QWidget
{
    Q_OBJECT

public:
    GoBangWidget(QWidget *parent = nullptr);
    ~GoBangWidget();

private:
    Ui::GoBangWidget *ui;
    int linesNum;
    double linesGap;
    QString blackChess;
    QString whiteChess;

    QFrame *boardFrame;
    QPoint offset;
    Game game;

    int clickX;
    int clickY;
    int moveX;
    int moveY;

    bool canPlay;
    bool canRepent;

    void setGameMsg(QString);
    void paintEvent(QPaintEvent *);
    void runGame();


public slots:
    void gameStart();
    void giveUpGame();
    void repentGame();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
//    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};
#endif // GOBANGWIDGET_H
