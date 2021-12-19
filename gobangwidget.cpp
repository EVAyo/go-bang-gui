#include "gobangwidget.h"
#include "ui_gobangwidget.h"
#include <QDebug>
#include <string>
#include <math.h>
GoBangWidget::GoBangWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GoBangWidget)
{
    ui->setupUi(this);
    linesNum = 15;
    linesGap = 30;
    clickX = 0;
    clickY = 0;
    blackChess = ":/chess/imgs/black.png";
    whiteChess = ":/chess/imgs/white.png";
    canPlay = false;

    /*main windows*/
    this->setFixedSize((linesNum-1)*linesGap+200,(linesNum-1)*linesGap+20);
    this->setWindowTitle("GoBang");
    this->setWindowIcon(QIcon(":/chess/imgs/logo.png"));

    /*board*/
    boardFrame = new QFrame(this);
    boardFrame->setObjectName("board");
//    qDebug() << boardFrame->objectName();
    boardFrame->setGeometry(QRect(10,10,(linesNum-1)*linesGap+1,(linesNum-1)*linesGap+1));
    boardFrame->setFrameShape(QFrame::Box);
    boardFrame->setLineWidth(2);
    boardFrame->raise();

    /*mouse event*/
    QCursor cursor;                      // 创建光标对象
//    cursor.setShape(Qt::OpenHandCursor); // 设置光标形状
    setCursor(cursor);
    /*Buttons tool*/

    connect(ui->gameStartBtn,SIGNAL(clicked()),this,SLOT(gameStart()));
    connect(ui->giveUpBtn,SIGNAL(clicked()),this,SLOT(giveUpGame()));
    connect(ui->repentGameBtn,SIGNAL(clicked()),this,SLOT(repentGame()));
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

    int **gridData = game.getGrid();
    for (int i=0; i<linesNum;i++) {
        for (int j=0; j<linesNum; j++) {
            int color = gridData[i][j];
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
//    printGrid(p);


    p.end();
}

void GoBangWidget::mousePressEvent(QMouseEvent *event) // 鼠标按下事件
{
    if(event->button() == Qt::LeftButton){       // 如果是鼠标左键按下
        QCursor cursor;
//        cursor.setShape(Qt::ClosedHandCursor);
        QApplication::setOverrideCursor(cursor); // 使鼠标指针暂时改变形状
        offset = event->globalPos() - pos();    // 获取指针位置和窗口位置的差值
//        ui->label_tip->setText();
        clickY = round((offset.x() - 10)/linesGap);
        clickX = round((offset.y() - 40)/linesGap);
        qDebug() << "X:" << clickX;
        qDebug() << "Y:" << clickY;
//        game.runGame(clickX,clickY);
//        update();
        qDebug()<<game.getCurUser();
        if(canPlay && clickX>=0&&clickX<linesNum&&clickY>=0&&clickY<linesNum){
            runGame();
        }
    }

}

//start the game
void GoBangWidget::gameStart()
{
    game.setWhoFirst(ui->firstWhoBox->currentIndex()==0?1:-1);
    game.initGrid();
    canPlay = true;
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
//    game.setGameMsg("恭喜" + res + "胜利!游戏结束!");
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
        game.setLastGrid(game.getGrid());
        game.printGrid();
        game.setGameMsg(game.getCurUser() == 1 ? "黑棋已落棋,请白棋行!" : "白棋已落棋,请黑棋行!");
        if (!game.putChess(clickX, clickY)) {
            game.putChess(clickX, clickY);
        }else{
            game.checkOver();
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

}
