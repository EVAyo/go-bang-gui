#include "gobangwidget.h"
#include "ui_gobangwidget.h"
#include "Online.h"
#include <QUdpSocket>
#include <QDebug>
#include <string>
#include <math.h>
#include <QHeaderView>
GoBangWidget::GoBangWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GoBangWidget)
{
    ui->setupUi(this);
    linesNum = 15;
    linesGap = 30;
    clickX = 0;
    clickY = 0;
    moveX = -1;
    moveY = -1;
    blackChess = ":/chess/imgs/black.png";
    whiteChess = ":/chess/imgs/white.png";
    canPlay = false;
    canRepent = false;

    /*main windows*/
    this->setFixedSize((linesNum-1)*linesGap+200,(linesNum-1)*linesGap+20);
    this->setWindowTitle("GoBang");
    this->setWindowIcon(QIcon(":/chess/imgs/logo.png"));
    this->setMouseTracking(true);

    /*board*/
    boardFrame = new QFrame(this);
    boardFrame->setObjectName("board");
    boardFrame->setGeometry(QRect(10,10,(linesNum-1)*linesGap+1,(linesNum-1)*linesGap+1));
    boardFrame->setFrameShape(QFrame::Box);
    boardFrame->setLineWidth(2);
    boardFrame->raise();
    boardFrame->setMouseTracking(true);

    /*Buttons tool*/
    connect(ui->gameStartBtn,SIGNAL(clicked()),this,SLOT(gameStart()));
    connect(ui->giveUpBtn,SIGNAL(clicked()),this,SLOT(giveUpGame()));
    connect(ui->repentGameBtn,SIGNAL(clicked()),this,SLOT(repentGame()));

    connect(ui->gameModeBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(selectGameMode()));

}

GoBangWidget::~GoBangWidget()
{
    delete ui;
}

void GoBangWidget::paintEvent(QPaintEvent *)
{
    QPainter p;
    p.begin(this);

    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::black);
    p.setPen(pen);
    //draw lines
    for(int n=0;n<linesNum-1;n++)
    {
        p.drawLine(10,linesGap*n+10,10+linesGap*(linesNum-1),linesGap*n+10);
        p.drawLine(linesGap*n+10,10,linesGap*n+10,10+linesGap*(linesNum-1));//Vertical
    }
    //draw points
    pen.setWidth(5);
    p.setPen(pen);
    p.drawPoint(10+linesGap*3,10+linesGap*3);
    p.drawPoint(10+linesGap*11,10+linesGap*3);
    p.drawPoint(10+linesGap*7,10+linesGap*7);
    p.drawPoint(10+linesGap*3,10+linesGap*11);
    p.drawPoint(linesGap*11+10,linesGap*11+10);

    //draw following points
    pen.setColor(Qt::red);
    pen.setWidth(5);
    p.setPen(pen);
    p.drawPoint(10+linesGap*moveY,10+linesGap*moveX);

    for (int i=0; i<linesNum;i++) {
        for (int j=0; j<linesNum; j++) {
            int color = game.getGrid()[i][j];
            QPixmap *chess = new QPixmap;
            if(color == 1){
                chess->load(blackChess);
                p.drawPixmap(j*linesGap-4,i*linesGap-4,28,28,*chess);
            }else if(color == -1){
                chess->load(whiteChess);
                p.drawPixmap(j*linesGap-4,i*linesGap-4,28,28,*chess);
            }
            delete chess;
        }
    }

    if(game.getWinPos()[2] != -1)
    {
        int r = game.getWinPos()[0];
        int c = game.getWinPos()[1];
        int dirc = game.getWinPos()[2];
        pen.setWidth(2);
        pen.setStyle(Qt::PenStyle::DashLine);//虚线
        pen.setColor(Qt::red);
        p.setPen(pen);
        switch (dirc) {
        case 1:	/*right*/
            p.drawLine(11+linesGap*c,11+linesGap*r,11+linesGap*(c+4),11+linesGap*r);
            break;
        case 2:	/*right-down*/
            p.drawLine(11+linesGap*c,11+linesGap*r,11+linesGap*(c+4),11+linesGap*(r+4));
            break;
        case 3:	/*down*/
            p.drawLine(11+linesGap*c,11+linesGap*r,11+linesGap*c,11+linesGap*(r+4));
            break;
        case 4:	/*left-down*/
            p.drawLine(11+linesGap*c,11+linesGap*r,11+linesGap*(c-4),11+linesGap*(r+4));
            break;
        default:
            break;
        }
    }

    p.end();
}

void GoBangWidget::mousePressEvent(QMouseEvent *event) // 鼠标按下事件
{
    if(event->button() == Qt::LeftButton){       // 如果是鼠标左键按下
        QCursor cursor;
        offset = event->globalPos() - pos();    // 获取指针位置和窗口位置的差值
        clickY = round((offset.x() - 10)/linesGap);
        clickX = round((offset.y() - 40)/linesGap);
        qDebug() << "X:" << clickX;
        qDebug() << "Y:" << clickY;
        qDebug()<<"CurUser:"<<game.getCurUser();
        if(canPlay && clickX>=0&&clickX<linesNum&&clickY>=0&&clickY<linesNum){
            cursor.setShape(Qt::CrossCursor);
            QApplication::setOverrideCursor(cursor); // 使鼠标指针暂时改变形状
            runGame();
            if(game.getGameMode()==2)
            {
                online->sendMessage(Online::ChessPos,ChessMsg{1,online->getMyIP(),clickX,clickY,game.getCurUser()});
            }
//            online->sendMessage(Online::ChessPos,{0,online->getMyIP(),clickX,clickY,game.getCurUser()});
            nextStep();
            runGame();
        }
    }
}

void GoBangWidget::nextStep()
{
    if(online->getNextPos().ip == online->getRivalIP())
    {
        clickX = online->getNextPos().r;
        clickY = online->getNextPos().c;
        game.setCurUser(online->getNextPos().color);
    }
}

void GoBangWidget::mouseReleaseEvent(QMouseEvent *) // 鼠标松开事件
{
    QCursor cursor;
    cursor.setShape(Qt::ArrowCursor);
    QApplication::setOverrideCursor(cursor); // 使鼠标指针暂时改变形状
}

void GoBangWidget::mouseMoveEvent(QMouseEvent *event) // 鼠标移动事件
{
    offset = event->globalPos() - pos();    // 获取指针位置和窗口位置的差值
    moveY = round((offset.x() - 10)/linesGap);
    moveX = round((offset.y() - 40)/linesGap);
    if(moveX>=0&&moveX<linesNum&&moveY>=0&&moveY<linesNum)
    {
//        qDebug() << "moveX:" << moveX;
//        qDebug() << "moveY:" << moveY;
        update();
    }
}

//start the game
void GoBangWidget::gameStart()
{
    game.setWhoFirst(ui->firstWhoBox->currentIndex()==0?1:-1);
    game.initGrid();
    canPlay = true;
    canRepent = true;
    ui->gameStartBtn->setText("重开");
    ui->giveUpBtn->setEnabled(true);
    setGameMsg(QString::fromStdString("请")+ui->firstWhoBox->currentText());
    update();
}

//give up the game
void GoBangWidget::giveUpGame()
{
    ui->giveUpBtn->setEnabled(false);
    std::string res = (game.getCurUser() == -1 ? "黑棋" : "白棋");
    game.setGameMsg("你认输了!恭喜" + res + "胜利!");
    setGameMsg(QString::fromStdString(game.getGameMsg()));
    update();
    canPlay = false;
}


//set game tip label content
void GoBangWidget::setGameMsg(QString msg)
{
    ui->label_tip->setText(msg);
}

//run game
void GoBangWidget::runGame()
{
    if(!game.getIsOver()){
        game.setGameMsg(game.getCurUser() == 1 ? "黑棋已落棋,请白棋行!" : "白棋已落棋,请黑棋行!");
        if (!game.putChess(clickX, clickY)) {
            game.putChess(clickX, clickY);
        }else{
            game.checkOver();
            canRepent = true;
//            if(game.getGameMode()==2)
//            {
//                online->sendMessage(Online::ChessPos,ChessMsg{1,online->getMyIP(),clickX,clickY,game.getCurUser()});
//            }
        }

    }else{
        std::string res = (game.getCurUser() == 1 ? "黑棋" : "白棋");
        game.setGameMsg("恭喜" + res + "胜利!游戏结束!");
        canPlay = false;
    }
    setGameMsg(QString::fromStdString(game.getGameMsg()));
    update();
}

//repent the game
void GoBangWidget::repentGame()
{
    if(!game.getIsOver())
    {

        if(canRepent)
        {
            game.setGrid(game.getLastGrid());
            game.setCurUser(game.getCurUser()*-1);
            game.setIsOver(false);
            canPlay = true;
            canRepent = false;
            game.setGameMsg(game.getCurUser() == 1 ? "黑棋悔棋,请重新落子!" : "白棋悔棋,请重新落子!");
        }
        else{
            game.setGameMsg("不能再悔棋了!");
        }
    }
    else
    {
        game.setGameMsg("棋局已结束，不能悔棋!");
    }
    setGameMsg(QString::fromStdString(game.getGameMsg()));
    update();
}

//select Game Mode
void GoBangWidget::selectGameMode()
{
    int lastMode = game.getGameMode();
    int newMode = ui->gameModeBox->currentIndex();
    switch (newMode) {
        case 0:
            if(lastMode == 2)
            {
                online->sendMessage(Online::ParticipantLeft);
                onlineOff();
            }
            game.setGameMode(0);

            break;
        case 1:
            if(lastMode == 2)
            {
                online->sendMessage(Online::ParticipantLeft);
                onlineOff();
            }
            game.setGameMode(1);
            break;
        case 2:
            gameStart();
            onlineGame();
            game.setGameMode(2);
            break;
    }
}

//start online game
void GoBangWidget::onlineGame()
{
//    online.sendMessage(Online::NewParticipant);

    online = new Online();
    connect(online->getSocket(), SIGNAL(readyRead()), this, SLOT(recieveMsg()));
    connect(ui->refreshBtn,SIGNAL(clicked()),this,SLOT(showOnlineUser()));

    online->init();

    showOnlineUser();
}

//close online game
void GoBangWidget::onlineOff()
{
    if(online->getOnlineState())
    {
        online->~Online();
        online = nullptr;
        //clear all item
        while (ui->onlineUserWidget->rowCount()>0)
        {
            ui->onlineUserWidget->removeRow(0);
        }
    }
}

void GoBangWidget::recieveMsg()
{
    online->processMsg();
    showOnlineUser();
    update();
}

void GoBangWidget::showOnlineUser()
{
    //clear all item
    while (ui->onlineUserWidget->rowCount()>0)
    {
        ui->onlineUserWidget->removeRow(0);
    }
    ui->onlineUserWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->onlineUserWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    std::vector<OnlineUser> * allUsers = online->getOnlineUser();
    for (int i=0;i<int(allUsers->size());i++)
    {
        QTableWidgetItem * ip = new QTableWidgetItem(allUsers->at(i).ipAddress);
        QPushButton *pBtn = new QPushButton();
        if(allUsers->at(i).isFree)
        {
            pBtn->setText("邀请");
        }else
        {
            pBtn->setText("忙碌");
            pBtn->setDisabled(true);
        }
        if(allUsers->at(i).ipAddress == online->getMyIP())
        {
            pBtn->setText("自己");
            pBtn->setDisabled(true);
        }
//        pBtn->setFixedSize(40,20);
        pBtn->setMaximumWidth(55);
        pBtn->setObjectName("userBtn_"+QString::number(i));
        connect(pBtn,SIGNAL(clicked()),this,SLOT(onlinePK()));

        ui->onlineUserWidget->insertRow(i);
        ui->onlineUserWidget->setItem(i,0,ip);
        ui->onlineUserWidget->setCellWidget(i,1,pBtn);
    }
    update();
}

//select a user to pk
void GoBangWidget::onlinePK()
{
    QString btnName = QObject::sender()->objectName();
    int index = (btnName.split("_")[1]).toInt();
    qDebug() << this->sender();
    qDebug() << index;

//    QString rivalIP = online->getOnlineUser()->at(index).ipAddress;
    online->setRivalIP(online->getOnlineUser()->at(index).ipAddress);
    qDebug() << "before pk";
    qDebug() << "rival" << online->getRivalIP();
    for (int i = 0; i < int(online->getOnlineUser()->size()); i++)
    {

        qDebug()<< online->getOnlineUser()->at(i).ipAddress;
        qDebug()<< online->getOnlineUser()->at(i).isFree;

    }

    online->sendMessage(Online::Invite);
    online->setUserState(online->getMyIP(),false);
    online->setUserState(online->getRivalIP(),false);
    showOnlineUser();
    qDebug() << "after pk";
    for (int i = 0; i < int(online->getOnlineUser()->size()); i++)
    {

        qDebug()<< online->getOnlineUser()->at(i).ipAddress;
        qDebug()<< online->getOnlineUser()->at(i).isFree;

    }
}
