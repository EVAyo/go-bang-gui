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
    enum GameState{WaitChess,DownChess,ForbidGame,OverGame,WarnGame};

private:
    Ui::GoBangWidget *ui;
    int zoom;
    int linesNum;
    double linesGap;
    QString blackChess;
    QString whiteChess;
    QString WaitChessImg;
    QString DownChessImg;
    QString ForbidGameImg;
    QString OverGameImg;
    QString WarningImg;

    QFrame *boardFrame;
    QPoint offset;
    Game game;
    Online * online;

    int clickX;
    int clickY;
    int moveX;
    int moveY;

    bool isStart;
    bool canPlay;
    bool canRepent;

    void setGameMsg(QString,GameState);
    void setRivalName(QString);
    int getMyColor();
    void paintEvent(QPaintEvent *);

    void runGame();
    void runOnlineGame();

    void nextStep();
    void onlineGameInit();
    void onlineGameStart(int);
    void onlineOff();

    void inviteProcess(QString,QString,int);
    void surrenderProcess(QString);
    void repentProcess(QString);
    void exceptionProcess(QString);

    void showGameOver();
    void showOnlineUser();
    void exceptionalGame();
    void winGame();
    void lostGame();

public slots:
    void selectGameMode();
    void gameStart();
    void giveUpGame();
    void repentGame();
    void invitePK();
    void recieveMsg();
    void refreshBtnClick();
    void aboutGame();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
//    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
};
#endif // GOBANGWIDGET_H
