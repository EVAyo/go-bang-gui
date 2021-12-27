#include "gobangwidget.h"
#include "ui_gobangwidget.h"
#include "Online.h"
#include <QUdpSocket>
#include <QHeaderView>
#include <QScreen>
#include <QMessageBox>
#include <QDebug>
#include <string>
#include <math.h>

GoBangWidget::GoBangWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GoBangWidget)
{
    ui->setupUi(this);
    zoom = qApp->primaryScreen()->logicalDotsPerInch()/96;
    qDebug() << zoom;

    linesNum = 15;
    linesGap = 30*zoom;
    clickX = 0;
    clickY = 0;
    moveX = -1;
    moveY = -1;
    blackChess = ":/chess/imgs/black.png";
    whiteChess = ":/chess/imgs/white.png";
    canPlay = false;
    canRepent = false;

    /*main windows*/
    this->setFixedSize((linesNum-1)*linesGap+220*zoom,(linesNum-1)*linesGap+20*zoom);
    this->setWindowTitle("GoBang");
    this->setWindowIcon(QIcon(":/chess/imgs/logo.png"));
    this->setMouseTracking(true);

    /*board*/
    boardFrame = new QFrame(this);
    boardFrame->setObjectName("board");
    boardFrame->setGeometry(QRect(10*zoom,10*zoom,(linesNum-1)*linesGap+1,(linesNum-1)*linesGap+1));
    boardFrame->setFrameShape(QFrame::Box);
    boardFrame->setLineWidth(2);
    boardFrame->raise();
    boardFrame->setMouseTracking(true);

    /*Function Frame*/
    ui->FunctionFrame->setGeometry(QRect((linesNum-1)*linesGap+21*zoom,10*zoom,190*zoom,(linesNum-1)*linesGap));
    ui->UserIco1->setMaximumSize(30*zoom,30*zoom);
    ui->UserIco2->setMaximumSize(30*zoom,30*zoom);
    setUserName("我自己","还是我自己");

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
        p.drawLine(10*zoom,linesGap*n+10*zoom,10*zoom+linesGap*(linesNum-1),linesGap*n+10*zoom);
        p.drawLine(linesGap*n+10*zoom,10*zoom,linesGap*n+10*zoom,10*zoom+linesGap*(linesNum-1));//Vertical
    }
    //draw points
    pen.setWidth(5*zoom);
    p.setPen(pen);
    p.drawPoint(10*zoom+linesGap*3,10*zoom+linesGap*3);
    p.drawPoint(10*zoom+linesGap*11,10*zoom+linesGap*3);
    p.drawPoint(10*zoom+linesGap*7,10*zoom+linesGap*7);
    p.drawPoint(10*zoom+linesGap*3,10*zoom+linesGap*11);
    p.drawPoint(10*zoom+linesGap*11,10*zoom+linesGap*11);

    //draw following points
    pen.setColor(Qt::red);
    pen.setWidth(5*zoom);
    p.setPen(pen);
    p.drawPoint(10*zoom+linesGap*moveY,10*zoom+linesGap*moveX);

    //draw chess
    for (int i=0; i<linesNum;i++) {
        for (int j=0; j<linesNum; j++) {
            int color = game.getGrid()[i][j];
            QPixmap *chess = new QPixmap;
            if(color == 1){
                chess->load(blackChess);
                p.drawPixmap(j*linesGap-4*zoom,i*linesGap-4*zoom,28*zoom,28*zoom,*chess);
            }else if(color == -1){
                chess->load(whiteChess);
                p.drawPixmap(j*linesGap-4*zoom,i*linesGap-4*zoom,28*zoom,28*zoom,*chess);
            }
            delete chess;
        }
    }
    //draw a line for win points
    if(game.getWinPos()[2] != -1)
    {
        int r = game.getWinPos()[0];
        int c = game.getWinPos()[1];
        int dirc = game.getWinPos()[2];
        pen.setWidth(2*zoom);
        pen.setStyle(Qt::PenStyle::DashLine);//虚线
        pen.setColor(Qt::red);
        p.setPen(pen);
        switch (dirc) {
        case 1:	/*right*/
            p.drawLine(11*zoom+linesGap*c,11*zoom+linesGap*r,11*zoom+linesGap*(c+4),11*zoom+linesGap*r);
            break;
        case 2:	/*right-down*/
            p.drawLine(11*zoom+linesGap*c,11*zoom+linesGap*r,11*zoom+linesGap*(c+4),11*zoom+linesGap*(r+4));
            break;
        case 3:	/*down*/
            p.drawLine(11*zoom+linesGap*c,11*zoom+linesGap*r,11*zoom+linesGap*c,11*zoom+linesGap*(r+4));
            break;
        case 4:	/*left-down*/
            p.drawLine(11*zoom+linesGap*c,11*zoom+linesGap*r,11*zoom+linesGap*(c-4),11*zoom+linesGap*(r+4));
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
        offset = event->globalPos() - pos();    // 获取指针位置和窗口位置的差值
        clickY = round((offset.x() - 10*zoom)/linesGap);
        clickX = round((offset.y() - 40*zoom)/linesGap);
        qDebug() << "X:" << clickX;
        qDebug() << "Y:" << clickY;
        qDebug()<<"CurUser:"<<game.getCurUser();
        if(canPlay && clickX>=0&&clickX<linesNum&&clickY>=0&&clickY<linesNum){
//            qDebug() << "game mode" << game.getGameMode();
            if(game.getGameMode()==2)
            {
                runOnlineGame();
            }
            else
            {
                runGame();
            }
        }
        else
        {
            setGameMsg(QString::fromStdString("不可以下棋!"));
        }
    }
    update();
}

void GoBangWidget::nextStep()
{
    qDebug() << "next step";
    if(online->getNextPos().ip == online->getRivalIP())
    {
        clickX = online->getNextPos().c;
        clickY = online->getNextPos().r;
        game.setCurUser(online->getNextPos().color);
        game.putChess(clickX, clickY);
        game.checkOver();
//        canPlay = false;
        canPlay= true;
    }
    if(!game.getIsOver())
    {
        canRepent = true;
        game.setGameMsg(online->getNextPos().color == 1 ? "黑棋已落棋,请白棋行!" : "白棋已落棋,请黑棋行!");
//        canPlay=true;
//        canPlay = false;
        game.setCurUser(online->getNextPos().color*-1);
    }
    else
    {
        game.setGameMsg("游戏结束!");
        if(getMyColor() == game.getCurUser())
        {
            winGame();
        }
        else
        {
            lostGame();
        }
        online->setUserState(online->getRivalIP(),true);
        showOnlineUser();
    }
    setGameMsg(QString::fromStdString(game.getGameMsg()));
    update();
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
    moveY = round((offset.x() - 10*zoom)/linesGap);
    moveX = round((offset.y() - 40*zoom)/linesGap);
    QCursor cursor;
    if(moveX>=0&&moveX<linesNum&&moveY>=0&&moveY<linesNum)
    {
//        qDebug() << "moveX:" << moveX;
//        qDebug() << "moveY:" << moveY;
        cursor.setShape(Qt::PointingHandCursor);
        QApplication::setOverrideCursor(cursor); // 使鼠标指针暂时改变形状
    }
    update();
}

//start the game
void GoBangWidget::gameStart()
{
    if(game.getGameMode() == 0)
    {
        game.setWhoFirst(ui->firstWhoBox->currentIndex()==0?1:-1);
        game.initGrid(0);
        canPlay = true;
        canRepent = true;
        ui->gameStartBtn->setText("重开");
        ui->giveUpBtn->setEnabled(true);
        setGameMsg(QString::fromStdString("请")+ui->firstWhoBox->currentText());
        update();
    }
    else if(game.getGameMode() == 1)
    {

    }
    else if(game.getGameMode() == 2)
    {
        canPlay = false;
        onlineGameStart(true,game.getWhoFirst());
    }
}

//start the online game
void GoBangWidget::onlineGameStart(bool isMaster,int masterColor)
{
    if(isMaster)    //master
    {
        masterColor==1?setUserName("我自己",online->getRivalIP()):
                       setUserName(online->getRivalIP(),"我自己");
        masterColor==1?setGameMsg(QString::fromStdString("请您先行棋!")):
                       setGameMsg(QString::fromStdString("等对方行棋!"));
        game.setWhoFirst(masterColor);
        canPlay = true;
    }
    else            //guest
    {
        masterColor==-1?setUserName("我自己",online->getRivalIP()):
                        setUserName(online->getRivalIP(),"我自己");
        masterColor==-1?setGameMsg(QString::fromStdString("请您先行棋!")):
                        setGameMsg(QString::fromStdString("等对方行棋!"));
        game.setWhoFirst(masterColor*-1);
        canPlay = false;
    }
    game.initGrid(2);
    online->setNextPos("",-1,-1,0);;
    canRepent = false;
    ui->repentGameBtn->setEnabled(false);
    ui->gameStartBtn->setText("重开");;
    ui->giveUpBtn->setEnabled(true);
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

//set user name label content
void GoBangWidget::setUserName(QString blackName,QString whiteName)
{
    ui->UserName1->setText(blackName);
    ui->UserName2->setText(whiteName);
}

//get who i am
int GoBangWidget::getMyColor()
{
    if(ui->UserName1->text() == "我自己")
    {
        return 1;
    }
    return -1;
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
        }
    }else{
        std::string res = (game.getCurUser() == 1 ? "黑棋" : "白棋");
        game.setGameMsg("恭喜" + res + "胜利!游戏结束!");
        canPlay = false;
    }
    if(game.getIsOver())
    {
        winGame();
    }
    setGameMsg(QString::fromStdString(game.getGameMsg()));
    update();
}

//run online game
void GoBangWidget::runOnlineGame()
{
    if(!game.getIsOver()){
        game.setGameMsg(game.getCurUser() == 1 ? "黑棋已落棋,请白棋行!" : "白棋已落棋,请黑棋行!");
        if (!game.putChess(clickX, clickY)) {
            game.putChess(clickX, clickY);
        }else{
            game.checkOver();
            canRepent = true;
        }
    }else{
        std::string res = (game.getCurUser() == 1 ? "黑棋" : "白棋");
        game.setGameMsg("恭喜" + res + "胜利!游戏结束!");
    }
    if(!game.getIsOver())
    {
        online->sendMessage(Online::ChessPos,{1,online->getMyIP(),"",clickX,clickY,game.getCurUser()*-1});
    }
    else
    {
        online->sendMessage(Online::ChessPos,{1,online->getMyIP(),"",clickX,clickY,game.getCurUser()});
    }
    canPlay = false;
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
            game.initGrid(0);
            setUserName("我自己","还是我自己");

            break;
        case 1:
            if(lastMode == 2)
            {
                online->sendMessage(Online::ParticipantLeft);
                onlineOff();
            }
            game.initGrid(1);
            setUserName("我自己","电脑AI");
            break;
        case 2:
            game.initGrid(2);
            onlineGame();
            setUserName("我自己","还没有人");
            break;
    }
    update();
}

//start online game
void GoBangWidget::onlineGame()
{
    online = new Online();
    connect(online->getSocket(), SIGNAL(readyRead()), this, SLOT(recieveMsg()));
    connect(ui->refreshBtn,SIGNAL(clicked()),this,SLOT(refreshBtnClick()));
    online->init();
    showOnlineUser();
    canPlay = false;
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
    ChessMsg CM = online->processMsg();
    switch (CM.msgType) {
        case 0:
            //ChessPos
            qDebug() << "recieveMsg ChessPos";
            nextStep();
            break;
        case 1:
            //NewParticipant
            break;
        case 2:
            //ParticipantLeft
            break;
        case 3:
            //Refresh
            break;
        case 4:
            //Invite
            qDebug() << "recieveMsg invite";
            inviteProcess(CM.ip1,CM.ip2,CM.color);
            break;
        case 5:
            //Accept
            break;
        case 6:
            //Refuse
            break;
        case -1:
            break;
    }
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
        pBtn->setMaximumWidth(45*zoom);
        pBtn->setObjectName("userBtn_"+QString::number(i));
        connect(pBtn,SIGNAL(clicked()),this,SLOT(invitePK()));

        ui->onlineUserWidget->insertRow(i);
        ui->onlineUserWidget->setItem(i,0,ip);
        QWidget *widget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout();
        layout->setMargin(0);
        layout->addWidget(pBtn);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        widget->setLayout(layout);
        ui->onlineUserWidget->setCellWidget(i,1,widget);
    }
    update();
}

//select a user to pk
void GoBangWidget::invitePK()
{
    QString btnName = QObject::sender()->objectName();
    int index = (btnName.split("_")[1]).toInt();

    game.setWhoFirst(ui->firstWhoBox->currentIndex()==0?1:-1);
    online->setRivalIP(online->getOnlineUser()->at(index).ipAddress);
    online->sendMessage(Online::Invite,{-1,online->getMyIP(),
                        online->getRivalIP(),-1,-1,game.getWhoFirst()});
    online->setUserState(online->getMyIP(),false);
    online->setUserState(online->getRivalIP(),false);
    onlineGameStart(true,game.getWhoFirst());
}

void GoBangWidget::inviteProcess(QString ip1,QString ip2,int color)
{
    if(ip2 == online->getMyIP())
    {
        QString content = ip1 + "邀请你对战!";
        int ret1 = QMessageBox::question(this, tr("对战邀请"),content,
                   QMessageBox::Yes, QMessageBox::No);
        if(ret1 == QMessageBox::Yes)
        {
            online->setRivalIP(ip1);
            online->setUserState(online->getMyIP(),false);
            online->setUserState(online->getRivalIP(),false);
            showOnlineUser();
            onlineGameStart(false,color);
        }
    }
}


// refresh user
void GoBangWidget::refreshBtnClick()
{
    if(game.getGameMode()==2)
    {
        online->cleanOnlineUser();
        online->sendMessage(Online::NewParticipant);
        showOnlineUser();
    }
}


//quit the game
void GoBangWidget::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Warning");
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIconPixmap(QPixmap(":/chess/imgs/cry.png"));
    msgBox.setText("真的要走吗？");
    msgBox.setInformativeText("…(｡•ˇ‸ˇ•｡)…");
    int ret = msgBox.exec();
    if(ret == QMessageBox::Yes)
    {
        qApp->quit();        //quit
        if(game.getGameMode() == 2)
        {
            online->sendMessage(Online::ParticipantLeft);
            onlineOff();
        }
    }
    else
    {
        event->ignore();;
    }
}

//win the game
void GoBangWidget::winGame()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Congratulation");
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setIconPixmap(QPixmap(":/chess/imgs/happy.png"));
    msgBox.setText("恭喜你赢了！");
    msgBox.setInformativeText("Ψ(￣∀￣)Ψ");
    msgBox.exec();
}
//lost the game
void GoBangWidget::lostGame()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("regretful");
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setIconPixmap(QPixmap(":/chess/imgs/sad.png"));
    msgBox.setText("差一点就赢了！");
    msgBox.setInformativeText("o(￣ヘ￣o＃)");
    msgBox.exec();
}
