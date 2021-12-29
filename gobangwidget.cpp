#include "gobangwidget.h"
#include "ui_gobangwidget.h"
#include "Online.h"
#include <QUdpSocket>
#include <QHeaderView>
#include <QScreen>
#include <QMessageBox>
#include <QFont>
#include <QMovie>
#include <QDebug>
#include <string>
#include <math.h>

GoBangWidget::GoBangWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GoBangWidget)
{
    ui->setupUi(this);
    zoom = qApp->primaryScreen()->logicalDotsPerInch()/96;
//    qDebug() << zoom;
    blackChess = ":/chess/imgs/black.png";
    whiteChess = ":/chess/imgs/white.png";
    WaitChessImg = ":/chess/imgs/wait.gif";
    DownChessImg = ":/chess/imgs/down.png";
    ForbidGameImg = ":/chess/imgs/forbid.png";
    OverGameImg = ":/chess/imgs/over.png";
    WarningImg = ":/chess/imgs/warn.png";

    linesNum = 15;
    linesGap = 30*zoom;
    clickX = -1;
    clickY = -1;
    moveX = -1;
    moveY = -1;
    isStart = false;
    canPlay = false;
    canRepent = false;

    /*main windows*/
    this->setFixedSize((linesNum-1)*linesGap+250*zoom,(linesNum-1)*linesGap+30*zoom);
    this->setWindowTitle("GoBang");
    this->setWindowIcon(QIcon(":/chess/imgs/logo.png"));
    this->setMouseTracking(true);

    /*Function Frame*/
    ui->FunctionFrame->setGeometry(QRect((linesNum-1)*linesGap+31*zoom,10*zoom,200*zoom,(linesNum-1)*linesGap));
    ui->UserIco1->setMaximumSize(30*zoom,30*zoom);
    ui->UserIco2->setMaximumSize(30*zoom,30*zoom);
    ui->waitContainer->setFixedSize(30*zoom,30*zoom);
    setUserName("我方","还是我方");

    /*Buttons tool*/
    connect(ui->gameModeBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(selectGameMode()));
    connect(ui->gameStartBtn,SIGNAL(clicked()),this,SLOT(gameStart()));
    connect(ui->giveUpBtn,SIGNAL(clicked()),this,SLOT(giveUpGame()));
    connect(ui->repentGameBtn,SIGNAL(clicked()),this,SLOT(repentGame()));
    connect(ui->aboutBtn,SIGNAL(clicked()),this,SLOT(aboutGame()));
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
    pen.setColor(QColor(162, 107, 62));
    p.setPen(pen);
    //draw lines
    for(int n=1;n<linesNum-1;n++)
    {
        p.drawLine(20*zoom,linesGap*n+10*zoom,20*zoom+linesGap*(linesNum-1),linesGap*n+10*zoom);
        p.drawLine(linesGap*n+20*zoom,10*zoom,linesGap*n+20*zoom,10*zoom+linesGap*(linesNum-1));//Vertical
    }
    pen.setColor(QColor(154, 127, 86));
    pen.setWidth(2);
    p.setPen(pen);
    p.drawRoundedRect(20*zoom,10*zoom,linesGap*(linesNum-1),linesGap*(linesNum-1),3,3);

    //draw points
    pen.setWidth(5*zoom);
    p.setPen(pen);
    p.drawPoint(20*zoom+linesGap*3,10*zoom+linesGap*3);
    p.drawPoint(20*zoom+linesGap*11,10*zoom+linesGap*3);
    p.drawPoint(20*zoom+linesGap*7,10*zoom+linesGap*7);
    p.drawPoint(20*zoom+linesGap*3,10*zoom+linesGap*11);
    p.drawPoint(20*zoom+linesGap*11,10*zoom+linesGap*11);

    //draw following points
    pen.setColor(QColor(162, 107, 62));
    pen.setWidth(3*zoom);
    p.setPen(pen);
    if(moveX>=0&&moveX<linesNum&&moveY>=0&&moveY<linesNum)
    {
        p.drawLine(20*zoom+linesGap*moveY,5*zoom+linesGap*moveX,20*zoom+linesGap*moveY,15*zoom+linesGap*moveX);//Vertical
        p.drawLine(15*zoom+linesGap*moveY,10*zoom+linesGap*moveX,25*zoom+linesGap*moveY,10*zoom+linesGap*moveX);
    }
    //draw chess
    for (int i=0; i<linesNum;i++) {
        for (int j=0; j<linesNum; j++) {
            int color = game.getGrid()[i][j];
            QPixmap *chess = new QPixmap();
            if(color == 1){
                chess->load(blackChess);
                p.drawPixmap(j*linesGap+6*zoom,i*linesGap-4*zoom,28*zoom,28*zoom,*chess);
            }else if(color == -1){
                chess->load(whiteChess);
                p.drawPixmap(j*linesGap+6*zoom,i*linesGap-4*zoom,28*zoom,28*zoom,*chess);
            }
            delete chess;
        }
    }

    pen.setColor(QColor(162, 107, 62));
    pen.setWidth(2);
    p.setPen(pen);
    if(clickX>=0&&clickX<linesNum&&clickY>=0&&clickY<linesNum)
    {
        p.drawRect(clickY*linesGap+6*zoom,clickX*linesGap-4*zoom,28*zoom,28*zoom);
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
            p.drawLine(21*zoom+linesGap*c,11*zoom+linesGap*r,21*zoom+linesGap*(c+4),11*zoom+linesGap*r);
            break;
        case 2:	/*right-down*/
            p.drawLine(21*zoom+linesGap*c,11*zoom+linesGap*r,21*zoom+linesGap*(c+4),11*zoom+linesGap*(r+4));
            break;
        case 3:	/*down*/
            p.drawLine(21*zoom+linesGap*c,11*zoom+linesGap*r,21*zoom+linesGap*c,11*zoom+linesGap*(r+4));
            break;
        case 4:	/*left-down*/
            p.drawLine(21*zoom+linesGap*c,11*zoom+linesGap*r,21*zoom+linesGap*(c-4),11*zoom+linesGap*(r+4));
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
        if(!(clickX>=0&&clickX<linesNum&&clickY>=0&&clickY<linesNum))
        {
            return;
        }
        if(canPlay){
            if(!game.checkPoint(clickX,clickY))
            {
                setGameMsg(QString::fromStdString("这里不可以落棋!"),ForbidGame);
                update();
                return;
            }
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
            setGameMsg(QString::fromStdString("未到我方下棋!"),WaitChess);
        }
       update();
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
    moveY = round((offset.x() - 10*zoom)/linesGap);
    moveX = round((offset.y() - 40*zoom)/linesGap);
    QCursor cursor;
    if(moveX>=0&&moveX<linesNum&&moveY>=0&&moveY<linesNum)
    {
        cursor.setShape(Qt::PointingHandCursor);
        QApplication::setOverrideCursor(cursor); // 使鼠标指针暂时改变形状
    }
    update();
}

//start the game
void GoBangWidget::gameStart()
{
    clickX = -1;
    clickY = -1;
    moveX = -1;
    moveY = -1;
    ui->gameStartBtn->setText("重开");
    ui->giveUpBtn->setEnabled(true);
    if(game.getGameMode() == 0)
    {
        isStart = true;
        canPlay = true;
        canRepent = true;
        game.setWhoFirst(ui->firstWhoBox->currentIndex()==0?1:-1);
        game.initGrid(0);
        QString msg = "请"+ui->firstWhoBox->currentText();
        setGameMsg(msg,WaitChess);
    }
    else if(game.getGameMode() == 1)
    {

    }
    else if(game.getGameMode() == 2)
    {
        if(!game.getIsOver())
        {
            exceptionalGame();
        }
        isStart = false;
        canPlay = false;
        onlineGameInit();
        onlineGameStart(true,game.getWhoFirst());
    }
    update();
}

//start the online game
void GoBangWidget::onlineGameStart(bool isMaster,int masterColor)
{
    game.initGrid(2);
    if(isStart)
    {
        if(isMaster)    //master
        {
            masterColor==1?setUserName("我方",online->getRivalIP()):
                           setUserName(online->getRivalIP(),"我方");
            masterColor==1?setGameMsg(QString::fromStdString("请您行棋!"),WaitChess):
                           setGameMsg(QString::fromStdString("等对方行棋!"),WaitChess);
            game.setWhoFirst(masterColor);
            canPlay = true;
        }
        else            //guest
        {
            masterColor==-1?setUserName("我方",online->getRivalIP()):
                            setUserName(online->getRivalIP(),"我方");
            masterColor==-1?setGameMsg(QString::fromStdString("请您行棋!"),WaitChess):
                            setGameMsg(QString::fromStdString("等对方行棋!"),WaitChess);
            game.setWhoFirst(masterColor*-1);
            canPlay = false;
        }
        canRepent = false;
        ui->repentGameBtn->setEnabled(true);
        ui->giveUpBtn->setEnabled(true);
        ui->gameStartBtn->setText("重开");
    }
    else
    {
        setGameMsg(QString::fromStdString("等待游戏开始!"),WaitChess);
    }
    update();
}


//set game tip label content
void GoBangWidget::setGameMsg(QString msg,GameState type)
{
    if(type == WaitChess)
    {
        QMovie *movie = new QMovie(WaitChessImg);
        movie->start();
        ui->waitContainer->setMovie(movie);
    }
    else
    {
        QImage *img = new QImage();
        switch (type) {
        case DownChess:
                img->load(DownChessImg);
            break;
        case ForbidGame:
                img->load(ForbidGameImg);
            break;
        case OverGame:
                img->load(OverGameImg);
            break;
        case WarnGame:
                img->load(WarningImg);
            break;
        default:
            break;
        }
        ui->waitContainer->setPixmap(QPixmap::fromImage(*img));
    }
    ui->label_tip->setText(msg);
}

//set user name label content
void GoBangWidget::setUserName(QString blackName,QString whiteName)
{
    if(blackName == "我方")
    {
        ui->UserName1->setText("<b>"+blackName+"</b>");
        ui->UserName2->setText(whiteName);
    }
    else
    {
        ui->UserName1->setText(blackName);
        ui->UserName2->setText("<b>"+whiteName+"</b>");
    }
}

//get who i am
int GoBangWidget::getMyColor()
{
    if(ui->UserName1->text() == "我方")
    {
        return 1;
    }
    return -1;
}

//run game
void GoBangWidget::runGame()
{
    if(!game.getIsOver()){
        QString msg = game.getCurUser() == 1 ? "请白棋行!" : "请黑棋行!";
        setGameMsg(msg,WaitChess);
        if (!game.putChess(clickX, clickY)) {
            game.putChess(clickX, clickY);
        }else{
            game.checkOver();
            canRepent = true;
        }
    }
    if(game.getIsOver())
    {
        canRepent = false;
        canPlay = false;
        showGameOver();
    }
    update();
}


//run online game
void GoBangWidget::runOnlineGame()
{
    if(!game.getIsOver()){
        if (!game.putChess(clickX, clickY)) {
            game.putChess(clickX, clickY);
        }else{
            game.checkOver();
        }
    }
    if(!game.getIsOver())
    {
        canRepent = true;
        online->sendMessage(Online::ChessPos,{1,online->getMyIP(),"",clickX,clickY,game.getCurUser()*-1});
        QString msg = "等对方行棋!";
        setGameMsg(msg,WaitChess);
    }
    else
    {
        online->sendMessage(Online::ChessPos,{1,online->getMyIP(),"",clickX,clickY,game.getCurUser()});
        isStart = false;
        canRepent = false;
        canPlay = false;
        showGameOver();
    }
    canPlay = false;
    update();
}
void GoBangWidget::nextStep()
{
    qDebug() << "next step";
    if(online->getNextPos().ip != online->getRivalIP())
    {
        return;
    }
    clickX = online->getNextPos().c;
    clickY = online->getNextPos().r;
    game.setCurUser(online->getNextPos().color);
    game.putChess(clickX, clickY);
    game.checkOver();
    if(!game.getIsOver())
    {
        canRepent = true;
        canPlay= true;
        QString msg = online->getNextPos().color != getMyColor() ? "请行棋!" : "等对方行棋!";
        setGameMsg(msg,WaitChess);
        game.setCurUser(online->getNextPos().color*-1);
    }
    else
    {
        canRepent = false;
        canPlay = false;
        isStart = false;
        showGameOver();
    }
    update();
}

//show Game Over
void GoBangWidget::showGameOver()
{
    if(game.getGameMode() == 0)
    {
        QString res = (game.getCurUser() == 1 ? "黑棋" : "白棋");
        QString msg = "恭喜" + res + "胜利!游戏结束!";
        setGameMsg(msg,OverGame);
        winGame();
    }
    if(game.getGameMode() == 2)
    {
        QString res = game.getWinPos()[3] == getMyColor()?"对手":"你";
        QString msg = "恭喜" + res + "赢了!游戏结束!";
        setGameMsg(msg,OverGame);
        game.getWinPos()[3] == getMyColor()?winGame():lostGame();
        online->setUserState(online->getRivalIP(),true);
        showOnlineUser();
    }
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
                if(!game.getIsOver())
                {
                    exceptionalGame();
                }
                else
                {
                    online->sendMessage(Online::ParticipantLeft);
                }
                onlineOff();
            }
            game.initGrid(0);
            setUserName("我方","还是我方");

            break;
        case 1:
            if(lastMode == 2)
            {
                if(!game.getIsOver())
                {
                    exceptionalGame();
                }
                else
                {
                    online->sendMessage(Online::ParticipantLeft);
                }
                onlineOff();
            }
            game.initGrid(1);
            setUserName("我方","电脑AI");
            break;
        case 2:
            game.initGrid(2);
            onlineGameInit();
            setUserName("我方","还没有人");
            break;
    }
    update();
}

//start online game
void GoBangWidget::onlineGameInit()
{
    online = new Online();
    connect(online->getSocket(), SIGNAL(readyRead()), this, SLOT(recieveMsg()));
    connect(ui->refreshBtn,SIGNAL(clicked()),this,SLOT(refreshBtnClick()));
    online->init();
    showOnlineUser();
    QString msg = "邀请对手开始对战!";
    setGameMsg(msg,DownChess);
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
            qDebug() << "recieveMsg Invite";
            inviteProcess(CM.ip1,CM.ip2,CM.color);
            break;
        case 5:
            //Accept
            break;
        case 6:
            //Refuse
            break;
        case 7:
            //Surrender
            qDebug() << "recieveMsg Surrender";
            surrenderProcess(CM.ip1);
            break;
        case 8 :
            qDebug() << "recieveMsg Repentance :";
            repentProcess(CM.ip1);
            break;
        case 9 :
            qDebug() << "recieveMsg Exception :";
            exceptionProcess(CM.ip1);
            break;
        case -1:
            break;
    }
    showOnlineUser();
    update();
}


//give up the game
void GoBangWidget::giveUpGame()
{
    if(!canRepent)
    {
        QString msg = "不能认输了!";
        setGameMsg(msg,ForbidGame);
        update();
        return;
    }
    ui->giveUpBtn->setEnabled(false);
    QString res = "";
    if(game.getGameMode() == 0)
    {
        res = (game.getCurUser() == -1 ? "黑棋" : "白棋");
    }
    else if(game.getGameMode() == 1)
    {

    }
    else if(game.getGameMode() == 2)
    {
        res = "对方";
        online->sendMessage(Online::Surrender);
        lostGame();
    }
    QString msg = "你认输了!恭喜" + res + "胜利!";
    setGameMsg(msg,OverGame);
    update();
    game.setIsOver(true);
    canPlay = false;
    canRepent = false;
    isStart = false;
}
//surrender Process
void GoBangWidget::surrenderProcess(QString ip)
{
    if(ip == online->getRivalIP())
    {
        online->setUserState(ip,false);
        online->setRivalIP("");
        isStart = false;
        canPlay = false;
        canRepent = false;
        game.setIsOver(true);
        QString msg = "对方认输!恭喜你赢了!";
        setGameMsg(msg,OverGame);
        update();
        winGame();
    }
}


//repent the game
void GoBangWidget::repentGame()
{
    if(!game.getIsOver()&&canRepent&&!canPlay)  //game not over&can repent&last is me
    {
        game.setGrid(game.getLastGrid());
        game.setCurUser(game.getCurUser()*-1);
        game.setIsOver(false);
        if(game.getGameMode() == 0)
        {
            QString msg = game.getCurUser() == 1 ? "黑棋悔棋,请重新落子!" : "白棋悔棋,请重新落子!";
            setGameMsg(msg,WaitChess);
        }
        else if(game.getGameMode() == 1)
        {

        }
        else if(game.getGameMode() == 2)
        {
            QString msg = "悔棋成功,请重新落子!";
            setGameMsg(msg,WaitChess);
            online->sendMessage(Online::Repentance);
        }
        canPlay = true;
        canRepent = false;
    }
    else
    {
        QString msg = "不能悔棋!";
        setGameMsg(msg,ForbidGame);
    }
    update();
}
//repentance Process
void GoBangWidget::repentProcess(QString ip)
{
    if(ip != online->getRivalIP())
    {
        return;
    }
    game.setGrid(game.getLastGrid());
    QString msg = "对方悔棋,等待重新落子!";
    setGameMsg(msg,WaitChess);
    canPlay = false;
    canRepent = false;
    update();
}


//exceptional Game
void GoBangWidget::exceptionalGame()
{
    online->sendMessage(Online::Exception);
    canPlay = false;
    canRepent = false;
    isStart = false;
    game.setIsOver(true);
}
//exception Process
void GoBangWidget::exceptionProcess(QString ip)
{
    if(ip != online->getRivalIP())
    {
        return;
    }
    QString msg = "对局异常,请重新开始!";
    setGameMsg(msg,WarnGame);
    canPlay = false;
    canRepent = false;
    isStart = false;
    game.setIsOver(true);
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
    isStart = true;
    onlineGameStart(true,game.getWhoFirst());
}
//Process a pk
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
            isStart = true;
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
        if(game.getGameMode() == 2)
        {
            if(!game.getIsOver())
            {
                exceptionalGame();
            }
            else
            {
                online->sendMessage(Online::ParticipantLeft);
            }
            onlineOff();
        }
        qApp->quit();        //quit
    }
    event->ignore();
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
//about the game
void GoBangWidget::aboutGame()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("about");
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setIconPixmap(QPixmap(":/chess/imgs/about.png"));
    msgBox.setText("<br>五子棋(x64)");
    msgBox.setInformativeText("<br>Linpure 2021<br>主页：<a href=\"https://gitee.com/linpure\" style='text-decoration:none;'>Gitee</a>");
    msgBox.exec();
}
