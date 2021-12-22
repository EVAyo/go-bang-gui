#ifndef ONLINE_H
#define ONLINE_H
#include <QString>
#include <vector>
class QUdpSocket;

struct OnlineUser{
    int id;
    QString hostName;
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

class Online
{
public:
    Online();
    ~Online();
    enum MessageType{ChessPos,NewParticipant,ParticipantLeft,Invite,Refuse};
//protected:
    void init();
    bool getOnlineState();
    void newParticipant(QString ipAddress);
    void participantLeft(QString ipAddress);
    void sendMessage(MessageType type,ChessMsg CMsg);
    ChessMsg processMsg();

    QString getMyIP();
    QString getRivalIP();
    void setRivalIP(QString);

    void addOnlineUser(QString,QString);
    void delOnlineUser(QString);
    std::vector<OnlineUser> * getOnlineUser();


    QString getIP();
    QString getUserName();
    QString getMessage();

private:
    QUdpSocket * udpSocket;
    qint16 port;
    bool onlineRun;
    const int MaxUserNums = 6;
    std::vector<OnlineUser> * onlineUsers;
    QString myHostName;
    QString myIpAddress;
    QString rivalIpAddress;

};

#endif // ONLINE_H
