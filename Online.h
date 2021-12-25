#ifndef ONLINE_H
#define ONLINE_H
#include <QString>
#include <vector>
class QUdpSocket;

struct OnlineUser{
    int id;
    QString ipAddress;
    bool isFree;    //free?can play:can not play
};
struct ChessMsg{
    int msgType;
    QString ip;
    int r;
    int c;
    int color;
};
struct NextPos{
    QString ip;
    int r;
    int c;
    int color;
};

class Online
{
public:
    Online();
    ~Online();
    enum MessageType{ChessPos,NewParticipant,ParticipantLeft,Invite,Refuse};
//protected:
    void init();
    QUdpSocket * getSocket();

    bool getOnlineState();

    void sendMessage(MessageType type,ChessMsg CMsg={-1,"",-1,-1,0});
    ChessMsg processMsg();

    QString getMyIP();
    QString getRivalIP();
    void setRivalIP(QString);

    void setUserState(QString,bool);
    bool getUserState(QString);



    void addOnlineUser(QString);
    void delOnlineUser(QString);
    std::vector<OnlineUser> * getOnlineUser();

    NextPos getNextPos();
    void setNextPos(QString,int,int,int);
    void processInvite(QString,QString);

    QString getIP();


private:
    QUdpSocket * udpSocket;
    qint16 port;
    bool onlineRun;
    const int MaxUserNums = 6;
    std::vector<OnlineUser> * onlineUsers;
    QString myHostName;
    QString myIpAddress;
    QString rivalIpAddress;
    NextPos nextPos;

};

#endif // ONLINE_H
