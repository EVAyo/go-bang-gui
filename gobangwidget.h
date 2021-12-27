#ifndef GOBANGWIDGET_H
#define GOBANGWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QPainter>
#include <QIcon>
#include <QMouseEvent>
#include "Game.h"
#include "Online.h"

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
    int zoom;
    int linesNum;
    double linesGap;
    QString blackChess;
    QString whiteChess;

    QFrame *boardFrame;
    QPoint offset;
    Game game;
    Online * online;

    int clickX;
    int clickY;
    int moveX;
    int moveY;

    bool canPlay;
    bool canRepent;

    void setGameMsg(QString);
    void setUserName(QString,QString);
    int getMyColor();
    void paintEvent(QPaintEvent *);
    void runGame();
    void runOnlineGame();

    void onlineGame();
    void onlineGameStart(bool,int);
    void onlineOff();
    void nextStep();

    void winGame();
    void lostGame();

public slots:
    void gameStart();
    void giveUpGame();
    void repentGame();
    void selectGameMode();

    void invitePK();
    void inviteProcess(QString,QString,int);

    void showOnlineUser();
    void recieveMsg();
    void refreshBtnClick();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
//    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
};
#endif // GOBANGWIDGET_H
