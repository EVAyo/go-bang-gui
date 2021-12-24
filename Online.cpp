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


void Online::init()
{
    myIpAddress = getIP();
    sendMessage(NewParticipant);
//    processMsg();
//    addOnlineUser(myHostName,myIpAddress);
}

//add Online User
void Online::addOnlineUser(QString ipAddress)
{
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
    qDebug() << "type out";
    switch(type)
    {
    case ChessPos :
        out << myIpAddress;
        out << CMsg.c << CMsg.r << CMsg.color;
        break;
    case NewParticipant :
        out << myIpAddress;
        break;
    case ParticipantLeft :
        out << myIpAddress;
        break;
    case Invite :
        out << myIpAddress;
        out << rivalIpAddress;
        break;
    case Refuse :
        break;
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port);
}



// recieve UDP msg
ChessMsg Online::processMsg()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        QDataStream in(&datagram, QIODevice::ReadOnly);
        qDebug() << "type in";
        int messageType;
        in >> messageType;
        QString ipAddress;
        int i,j,color;
        switch(messageType)
        {
        case ChessPos:
            in >> ipAddress >> i>>j>>color;
            qDebug() << ipAddress;
            qDebug() << i<<j<<color;
            return {0,ipAddress,i,j,color};
            break;

        case NewParticipant:
            in >>ipAddress;
//            newParticipant(userName,ipAddress);
            addOnlineUser(ipAddress);
            qDebug() << ipAddress;
            return {1,ipAddress,-1,-1,0};
            break;

        case ParticipantLeft:
            in >> ipAddress;
//            participantLeft(userName,localHostName);
            qDebug() << ipAddress;
            return {2,ipAddress,-1,-1,0};
            break;

        case Invite:
            break;

        case Refuse:
            break;
        }
    }
    return {-1,"",-1,-1,0};
}


//new user participant
void Online::participantLeft(QString ipAddress)
{

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
