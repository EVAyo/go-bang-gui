#include "Online.h"
#include <QUdpSocket>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QDebug>

Online::Online()
{
    onlineRun = true;
    udpSocket = new QUdpSocket();
    port = 8888;
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
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
//clean Online User
void Online::cleanOnlineUser()
{
    if(!onlineUsers->empty())
    {
    onlineUsers->clear();
    }
}

// send  UDP msg
void Online::sendMessage(MessageType type,ChessMsg CMsg)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << type;
    qDebug() << type;
    switch(type)
    {
    case ChessPos :
        qDebug() << "out ChessPos:";
        out << CMsg.ip1;
        out << CMsg.c << CMsg.r << CMsg.color;
        break;
    case NewParticipant :
        qDebug() << "out NewParticipant:";
        out << getMyIP();
        break;
    case ParticipantLeft :
        qDebug() << "out ParticipantLeft:";
        out << getMyIP();
        break;
    case Refresh:
        qDebug() << "out Refresh:";
        out << getMyIP();
        break;
    case Invite :
        qDebug() << "out Invite:";
        out << getMyIP() << getRivalIP();
        out << CMsg.color;
        break;
    case Accept:
        break;
    case Refuse :
        break;
    case Surrender:
        qDebug() << "out Surrender:";
        out << getMyIP();
        break;
    case Repentance :
        qDebug() << "out Repentance :";
        out << getMyIP();
        break;
    case Exception :
        qDebug() << "out Exception :";
        out << getMyIP();
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
            return {0,ipAddress,"",i,j,color};
            break;

        case NewParticipant:
            in >>ipAddress;
            qDebug() << "in NewParticipant:";
            addOnlineUser(ipAddress);
            sendMessage(Refresh);
            break;

        case ParticipantLeft:
            in >> ipAddress;
            qDebug() << "in ParticipantLeft:";
            delOnlineUser(ipAddress);
            break;
        case Refresh:
            in >> ipAddress;
            addOnlineUser(ipAddress);
            break;
        case Invite:
            in >> myIP >> rivalIP;
            qDebug() << "in Invite:";
            return {4,myIP,rivalIP,-1,-1,0};
            break;
        case Accept:
            return {5,ipAddress,"",-1,-1,0};
            break;
        case Refuse:
            in >> ipAddress;
            qDebug() << ipAddress;
            return {6,ipAddress,"",-1,-1,0};
            break;
        case Surrender:
            in >> ipAddress;
            qDebug() << ipAddress;
            return {7,ipAddress,"",-1,-1,0};
            break;
        case Repentance :
            in >> ipAddress;
            qDebug() << ipAddress;
            return {8,ipAddress,"",-1,-1,0};
            break;
        case Exception :
            in >> ipAddress;
            qDebug() << ipAddress;
            return {9,ipAddress,"",-1,-1,0};
            break;
        }
    }
    return {-1,"","",-1,-1,0};
}


// get ip
QString Online::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;
}
