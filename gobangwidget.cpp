#include "gobangwidget.h"
#include "ui_gobangwidget.h"
#include "Online.h"
#include "ChessEngine.h"
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

    /*Buttons tool*/
    connect(ui->gameModeBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(selectGameMode()));
    connect(ui->gameStartBtn,SIGNAL(clicked()),this,SLOT(gameStart()));
    connect(ui->giveUpBtn,SIGNAL(clicked()),this,SLOT(giveUpGame()));
    connect(ui->repentGameBtn,SIGNAL(clicked()),this,SLOT(repentGame()));
    connect(ui->aboutBtn,SIGNAL(clicked()),this,SLOT(aboutGame()));
    connect(ui->manualBtn,SIGNAL(clicked()),this,SLOT(manualGame()));
}

GoBangWidget::~GoBangWidget()
{
    delete ui;
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
void GoBangWidget::setRivalName(QString rivalName)
{
    ui->UserName2->setText(rivalName);
    ui->UserIco1->setPixmap(QPixmap(blackChess));
    ui->UserIco2->setPixmap(QPixmap(whiteChess));
    qDebug()<<game.getGameMode();
    if(game.getGameMode()!=2)
    {
        return;
    }
    if(!online->getIsMaster())
    {
        ui->UserIco1->setPixmap(QPixmap(whiteChess));
        ui->UserIco2->setPixmap(QPixmap(blackChess));
    }
}
//get who i am
int GoBangWidget::getMyColor()
{
    if(online->getIsMaster())
    {
        return 1;
    }
    return -1;
}
//paintEvent
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


    QBrush brush;
    brush.setColor(QColor(162, 107, 62, 50));
    brush.setStyle(Qt::SolidPattern);
    p.setBrush(brush);
    pen.setColor(QColor(154, 127, 86));
    pen.setWidth(2);
    p.setPen(pen);
    p.drawRoundedRect(20*zoom,10*zoom,linesGap*(linesNum-1),linesGap*(linesNum-1),3,3);
    p.setBrush(QBrush(Qt::NoBrush));

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
        pen.setStyle(Qt::PenStyle::DashLine);//??????
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

//mousePressEvent
void GoBangWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        offset = event->globalPos() - pos();
        clickY = round((offset.x() - 20*zoom)/linesGap);
        clickX = round((offset.y() - 40*zoom)/linesGap);

        if(!(clickX>=0&&clickX<linesNum&&clickY>=0&&clickY<linesNum))
        {
            return;
        }
        qDebug() << "X:" << clickX;
        qDebug() << "Y:" << clickY;
        qDebug()<<"CurUser:"<<game.getCurUser();
        if(canPlay){
            if(!game.checkPoint(clickX,clickY))
            {
                setGameMsg(QString::fromStdString("?????????????????????!"),ForbidGame);
                update();
                return;
            }
            if(game.getGameMode()==2)
            {
                runOnlineGame();
            }
            else if(game.getGameMode() == 1)
            {
                runAIGame();
            }
            else
            {
                runGame();
            }
        }
        else
        {
            setGameMsg(QString::fromStdString("???????????????!"),ForbidGame);
        }
       update();
    }
}
// mouseReleaseEvent
void GoBangWidget::mouseReleaseEvent(QMouseEvent *)
{
    QCursor cursor;
    cursor.setShape(Qt::ArrowCursor);
    QApplication::setOverrideCursor(cursor);
}
// mouseMoveEvent
void GoBangWidget::mouseMoveEvent(QMouseEvent *event)
{
    offset = event->globalPos() - pos();
    moveY = round((offset.x() - 20*zoom)/linesGap);
    moveX = round((offset.y() - 40*zoom)/linesGap);
    QCursor cursor;
    if(moveX>=0&&moveX<linesNum&&moveY>=0&&moveY<linesNum)
    {
        cursor.setShape(Qt::PointingHandCursor);
        QApplication::setOverrideCursor(cursor);
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
    if(game.getGameMode() == 0)
    {
        isStart = true;
        canPlay = true;
        canRepent = true;
        game.setWhoFirst(ui->firstWhoBox->currentIndex()==0?1:-1);
        game.initGrid(0);
        QString msg = "???"+ui->firstWhoBox->currentText();
        setGameMsg(msg,WaitChess);
    }
    else if(game.getGameMode() == 1)
    {
        game.initGrid(1);
        game.setWhoFirst(ui->firstWhoBox->currentIndex()==0?1:-1);
        selectAILevel();
        isStart = true;
        canPlay = true;
        canRepent = true;
        if(game.getWhoFirst()==1)
        {
            setGameMsg("????????????",WaitChess);
            ChessEngine::reset(0);
        }
        else
        {
            game.setCurUser(-1);
            ChessEngine::reset(1);
            clickX = ChessEngine::getLastPosition().x;
            clickY = ChessEngine::getLastPosition().y;
            qDebug() << "AIx" << clickX;
            qDebug() << "AIy" << clickY;
            game.putChess(clickX, clickY);
            setGameMsg("????????????",WaitChess);
            game.setCurUser(1);
        }
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
        onlineGameStart(game.getWhoFirst());
    }
    ui->repentGameBtn->setEnabled(true);
    ui->giveUpBtn->setEnabled(true);
    ui->gameStartBtn->setText("??????");
    update();
}
//start the online game
void GoBangWidget::onlineGameStart(int masterColor)
{
    game.initGrid(2);
    if(isStart)
    {
        if(masterColor == getMyColor())     //first
        {
            setGameMsg(QString::fromStdString("????????????!"),WaitChess);
            canPlay = true;
        }
        else                                //second
        {
            setGameMsg(QString::fromStdString("???????????????!"),WaitChess);
            canPlay = false;
        }
        game.setCurUser(masterColor);
        canRepent = false;
    }
    else
    {
        setGameMsg(QString::fromStdString("??????????????????!"),WaitChess);
    }
    online->getRivalIP()!=""?setRivalName(online->getRivalIP()):setRivalName("????????????");
    update();
}

//run game
void GoBangWidget::runGame()
{
    if(!game.getIsOver()){
        QString msg = game.getCurUser() == 1 ? "????????????!" : "????????????!";
        setGameMsg(msg,WaitChess);
        if (!game.putChess(clickX, clickY)) {
            game.putChess(clickX, clickY);
        }
        game.checkOver();
        canRepent = true;
        canPlay = true;
    }
    if(game.getIsOver())
    {
        canRepent = false;
        canPlay = false;
        showGameOver();
    }
    update();
}
//run AI game
void GoBangWidget::runAIGame()
{
    if(!game.getIsOver())
    {
        if (!game.putChess(clickX, clickY)) {
            game.putChess(clickX, clickY);
        }
        game.checkOver();
        canRepent = true;
        canPlay = true;
        setGameMsg("?????????????????????",WaitChess);
        repaint();
    }

    if(game.getIsOver())
    {
        canRepent = false;
        canPlay = false;
        isStart = false;
        showGameOver();
    }
    else
    {
        ChessEngine::nextStep(clickX,clickY);
        clickX = ChessEngine::getLastPosition().x;
        clickY = ChessEngine::getLastPosition().y;
        qDebug() << "AIx" << clickX;
        qDebug() << "AIy" << clickY;
        game.putChess(clickX, clickY);
        game.checkOver();
        if(game.getIsOver())
        {
            canRepent = false;
            canPlay = false;
            isStart = false;
            showGameOver();
        }
        else
        {
            setGameMsg("????????????",WaitChess);
        }
    }
    update();
}

//run online game
void GoBangWidget::runOnlineGame()
{
    if(!game.getIsOver()){
        if (!game.putChess(clickX, clickY)) {
            game.putChess(clickX, clickY);
        }
        game.checkOver();
    }
    if(!game.getIsOver())
    {
        canRepent = true;
        online->sendMessage(Online::ChessPos,{1,online->getMyIP(),"",clickX,clickY,game.getCurUser()*-1});
        QString msg = "???????????????!";
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

//recieve rival chess positon
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
        canPlay= true;
        QString msg = "?????????!";
        setGameMsg(msg,WaitChess);
        game.setCurUser(online->getNextPos().color*-1);
    }
    else
    {
        canPlay = false;
        isStart = false;
        showGameOver();
    }
    canRepent = false;
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
    QString msg = "????????????????????????!";
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
//process recieveMsg
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
//            setGameMsg(CM.ip1+"?????????!",DownChess);
//            update();
            break;
        case 2:
            //ParticipantLeft
//            setGameMsg(CM.ip1+"?????????!",DownChess);
//            update();
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

//show Game Over
void GoBangWidget::showGameOver()
{
    ui->gameModeBox->setDisabled(false);
    ui->difficultyBox->setDisabled(false);
    ui->firstWhoBox->setDisabled(false);
    if(game.getGameMode() == 0)
    {
        QString res = (game.getCurUser() == 1 ? "??????" : "??????");
        QString msg = "??????" + res + "??????!????????????!";
        setGameMsg(msg,OverGame);
        winGame();
    }
    if(game.getGameMode() == 1)
    {
        if(game.getCurUser() == 1)
        {
            setGameMsg("???????????????!",OverGame);
            winGame();
        }
        else
        {
            setGameMsg("?????????!????????????!",OverGame);
            lostGame();
        }
        update();
    }
    if(game.getGameMode() == 2)
    {
        QString msg = "???????????????!????????????!";
        if(game.getWinPos()[3] == getMyColor())
        {
            winGame();
        }
        else
        {
            lostGame();
            msg = "???????????????!????????????!";
        }
        setGameMsg(msg,OverGame);
        online->setUserState(online->getRivalIP(),true);
        showOnlineUser();
    }
}
//show Online User
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
            pBtn->setText("??????");
        }else
        {
            pBtn->setText("??????");
            pBtn->setDisabled(true);
        }
        if(allUsers->at(i).ipAddress == online->getMyIP())
        {
            pBtn->setText("??????");
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
            setRivalName("????????????");
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
            setRivalName("??????AI");
            ChessEngine::beforeStart();
            break;
        case 2:
            game.initGrid(2);
            onlineGameInit();
            setRivalName("????????????");
            break;
    }
    update();
}
//select AI Level
void GoBangWidget::selectAILevel()
{
    int level = ui->difficultyBox->currentIndex();
    switch (level)
    {
    case 0:
        ChessEngine::setLevel(1);
    break;
    case 1:
        ChessEngine::setLevel(3);
    break;
    case 2:
        ChessEngine::setLevel(4);
    break;
    case 3:
        ChessEngine::setLevel(5);
    break;
    case 4:
        ChessEngine::setLevel(6);
    break;
    case 5:
        ChessEngine::setLevel(7);
    break;
    }
}

//give up the game
void GoBangWidget::giveUpGame()
{
    if(!canRepent)
    {
        QString msg = "???????????????!";
        setGameMsg(msg,ForbidGame);
        update();
        return;
    }
    ui->giveUpBtn->setEnabled(false);
    QString res = "";
    if(game.getGameMode() == 0)
    {
        res = (game.getCurUser() == 1 ? "??????" : "??????");
    }
    else if(game.getGameMode() == 1)
    {
        res = "??????";
        lostGame();
    }
    else if(game.getGameMode() == 2)
    {
        res = "??????";
        online->sendMessage(Online::Surrender);
        lostGame();
    }
    QString msg = "????????????!??????" + res + "??????!";
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
        QString msg = "????????????!???????????????!";
        setGameMsg(msg,OverGame);
        update();
        winGame();
    }
}

//repent the game
void GoBangWidget::repentGame()
{
    if(!game.getIsOver()&&canRepent)  //game not over&can repent&last is me
    {
        game.setGrid(game.getLastGrid());
        game.setCurUser(game.getCurUser()*-1);
        game.setIsOver(false);
        if(game.getGameMode() == 0)
        {
            QString msg = game.getCurUser() == 1 ? "????????????,???????????????!" : "????????????,???????????????!";
            setGameMsg(msg,WaitChess);
        }
        else if(game.getGameMode() == 1)
        {
            string backBoard = ChessEngine::takeBack();
            int **boardArr = new int*[linesNum];
            for (int i = 0; i < linesNum; i++) {
                boardArr[i] = new int[linesNum];
            }
            for (int i = 0; i < linesNum; i++) {
                for (int j = 0; j < linesNum; j++)
                {
                    if(backBoard[i*linesNum+j]=='1'){
                        boardArr[i][j] = 1;
                    }else if(backBoard[i*linesNum+j]=='2'){
                        boardArr[i][j] = -1;
                    }else{
                        boardArr[i][j] = 0;
                    }
                }
            }
            game.setGrid(boardArr);
            game.setCurUser(game.getCurUser()*-1);
            QString msg = "????????????,???????????????!";
            setGameMsg(msg,WaitChess);
        }
        else if(game.getGameMode() == 2/*&&!canPlay*/)
        {
            QString msg = "????????????,???????????????!";
            setGameMsg(msg,WaitChess);
            online->sendMessage(Online::Repentance);
        }
        canPlay = true;
        canRepent = false;
    }
    else
    {
        QString msg = "????????????!";
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
    QString msg = "????????????,??????????????????!";
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
    QString msg = "????????????,???????????????!";
    setGameMsg(msg,WarnGame);
    canPlay = false;
    canRepent = false;
    isStart = false;
    game.setIsOver(true);
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
    game.getWhoFirst() == 1?isStart = true:isStart = false;
    ui->firstWhoBox->setDisabled(true);
    ui->gameStartBtn->setText("??????");
    onlineGameStart(game.getWhoFirst());
}
//Process a pk
void GoBangWidget::inviteProcess(QString ip1,QString ip2,int color)
{
    if(ip2 == online->getMyIP())
    {
        QString content = ip1 + "???????????????!";
        int ret1 = QMessageBox::question(this, tr("????????????"),content,
                   QMessageBox::Yes, QMessageBox::No);
        if(ret1 == QMessageBox::Yes)
        {
            online->setRivalIP(ip1);
            online->setUserState(online->getMyIP(),false);
            online->setUserState(online->getRivalIP(),false);
            online->setIsMaster(false);
            showOnlineUser();
            color == -1?isStart = true:isStart = false;
            ui->firstWhoBox->setCurrentIndex(isStart?1:0);
            ui->firstWhoBox->setDisabled(true);
            ui->gameStartBtn->setText("??????");
            game.setWhoFirst(color);
            onlineGameStart(color);
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
    msgBox.setText("??????????????????");
    msgBox.setInformativeText("???(???????????????????)???");
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
    msgBox.setText("??????????????????");
    msgBox.setInformativeText("??(?????????)??");
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
    msgBox.setText("?????????????????????");
    msgBox.setInformativeText("o(?????????o???)");
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
    msgBox.setText("<br>?????????(x64)");
    msgBox.setInformativeText("Base:C++ Qt<br>By:Linpure<br>Date:2021-12<br><a href=\"https://gitee.com/linpure\">Gitee/Linpure</a><br><a href=\"https://github.com/kimlongli/FiveChess\" style='text-decoration:none;'>AI Reference</a>");
    msgBox.exec();
}
//show game manual
void GoBangWidget::manualGame()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("help");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);
    QString manual = "<p>   ???????????????????????????<br>"
                     "1.??????????????????????????????????????????<br>"
                     "2.??????????????????????????????????????????<br>"
                     "3.??????????????????????????????????????????<br>"
                     "4.??????????????????????????????????????????<br>"
                     "5.????????????bug?????????????????????<br>"
                     "</p>";
    msgBox.setText(manual);
    msgBox.exec();
}
