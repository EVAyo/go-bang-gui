#include "Online.h"
#include <QUdpSocket>
#include <QHostInfo>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QDebug>

Online::Online()
{
    onlineRun = true;
    udpSocket = new QUdpSocket();
    port = 8888;
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
//    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

    //    sendMessage(NewParticipant);
    onlineUsers = new std::vector<OnlineUser>[MaxUserNums];
}

Online::~Online()
{
    onlineRun = false;
    udpSocket->close();
    delete udpSocket;
    udpSocket = nullptr;
}

void Online::init()
{
    setNextPos("",-1,-1,0);
    myIpAddress = getIP();
    sendMessage(NewParticipant);
//    processMsg();
//    addOnlineUser(myHostName,myIpAddress);
}

QUdpSocket * Online::getSocket()
{
    return udpSocket;
}

bool Online::getOnlineState()
{
    return onlineRun;
}

//get my ip
QString Online::getMyIP()
{
    return myIpAddress;
}

//get rival ip
QString Online::getRivalIP()
{
    return rivalIpAddress;
}
//set rival ip
void Online::setRivalIP(QString ip)
{
    rivalIpAddress = ip;
}

//set User State
void Online::setUserState(QString ip,bool val)
{
    for (int i = 0; i < int(onlineUsers->size()); i++)
    {
        if (onlineUsers->at(i).ipAddress == ip)
        {
            onlineUsers->at(i).isFree = val;
        }
    }
}
//get User State
bool Online::getUserState(QString ip)
{
    for (int i = 0; i < int(onlineUsers->size()); i++)
    {
        if (onlineUsers->at(i).ipAddress == ip)
        {
            return onlineUsers->at(i).isFree;
        }
    }
    return false;
}

//get Next Pos
NextPos Online::getNextPos()
{
    return nextPos;
}
//set Next Pos
void Online::setNextPos(QString ip,int i,int j,int color)
{
    nextPos = {ip,i,j,color};
}


//add Online User
void Online::addOnlineUser(QString ipAddress)
{
    for (int i = 0; i < int(onlineUsers->size()); i++)
    {
        if (onlineUsers->at(i).ipAddress == ipAddress)
        {
            return;
        }
    }
    int nums = onlineUsers->size();
    onlineUsers->push_back({nums+1,ipAddress,true});
}
//get Online User
std::vector<OnlineUser> * Online::getOnlineUser()
{
    return onlineUsers;
}
//delete Online User
void Online::delOnlineUser(QString ipAddress)
{
    for (int i = 0; i < int(onlineUsers->size()); i++)
    {
        if (onlineUsers->at(i).ipAddress == ipAddress)
        {
            /* code */
            onlineUsers->erase(onlineUsers->begin()+i);
            i--;
        }
    }
}


// send  UDP msg
void Online::sendMessage(MessageType type,ChessMsg CMsg)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << type;
    switch(type)
    {
    case ChessPos :
        qDebug() << "out ChessPos:";
        out << myIpAddress;
        out << CMsg.c << CMsg.r << CMsg.color;
        break;
    case NewParticipant :
        qDebug() << "out NewParticipant:";
        out << myIpAddress;
        break;
    case ParticipantLeft :
        qDebug() << "out ParticipantLeft:";
        out << myIpAddress;
        break;
    case Invite :
        qDebug() << "out Invite:";
        out << myIpAddress << rivalIpAddress;
        break;
    case Refuse :
        break;
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port);
}



// recieve UDP msg
ChessMsg Online::processMsg()
{
    QByteArray datagram;
    while(udpSocket->hasPendingDatagrams())
    {
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        udpSocket->readDatagram(datagram.data(), datagram.size());
        QDataStream in(&datagram, QIODevice::ReadOnly);
//        qDebug() << "type in";
        int messageType;
        in >> messageType;
        QString ipAddress;
        QString myIP;
        QString rivalIP;
        int i,j,color;
        switch(messageType)
        {
        case ChessPos:
            in >> ipAddress >> i>>j>>color;
            qDebug() << "in ChessPos:";
            qDebug() << ipAddress;
            qDebug() << i<<j<<color;
            setNextPos(ipAddress,i,j,color);
            return {0,ipAddress,i,j,color};
            break;

        case NewParticipant:
            in >>ipAddress;
            qDebug() << "in NewParticipant:";
//            newParticipant(userName,ipAddress);
            addOnlineUser(ipAddress);
            qDebug() << ipAddress;
            return {1,ipAddress,-1,-1,0};
            break;

        case ParticipantLeft:
            in >> ipAddress;
            qDebug() << "in ParticipantLeft:";
//            participantLeft(userName,localHostName);
            delOnlineUser(ipAddress);
            qDebug() << ipAddress;
            return {2,ipAddress,-1,-1,0};
            break;

        case Invite:
            in >> myIP >> rivalIP;
            qDebug() << "in Invite:";
            processInvite(myIP,rivalIP);
            break;

        case Refuse:
            in >> ipAddress;
            qDebug() << ipAddress;
            break;
        }
    }
    return {-1,"",-1,-1,0};
}

void Online::processInvite(QString myIP,QString rivalIP)
{
    addOnlineUser(myIP);
    if(rivalIP == myIpAddress)
    {
        qDebug() << "i am rival";
        qDebug() << myIpAddress;
        setRivalIP(myIP);
        setUserState(myIP,false);
        setUserState(rivalIP,false);
    }
    else
    {
        qDebug() << "i am mine";
        qDebug() << myIpAddress;
    }
}



// 获取ip地址
QString Online::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;
}
