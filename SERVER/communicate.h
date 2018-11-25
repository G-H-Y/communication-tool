#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include "server.h"
#include "sockets.h"
#include "dealuserinfo.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTcpSocket>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
class SERVER;

class COMMUNICATE{
public:
    enum Request{
        SIGN_IN, //登录
        SIGN_UP, //注册
        SIGN_OUT,
        FIND_PSD,
        SEND_MSG,
        LINK_FRIEND,
        ADD_FRIEND
    };
    enum Result{
        NO_ERROR,
        USER_EXIST,
        USER_NOEXIST,
        PSD_WRONG,
        ANSWER_WRONG,
        USER_OFFLINE,
        UNKNOWN,        
    };
    SERVER *myServer;
    QJsonObject object;
    SocketInfo *sktInfo;
    COMMUNICATE(SERVER *myServer,QJsonObject &object,SocketInfo *sktInfo);
    void SignIn();
    void SignUp();
    void SignOut();
    void InformFriend(const QString &userName,
                      const QString &ip,int udpPort,int tcpPort);
    void FindPsd();
    void SendMsg();
    void SendOffMsg(QJsonArray  &sendOffMsg);
    void LinkFriend();
    void AddFriend();
    static void Send(QTcpSocket *socket, QJsonObject &sendObject);
};

#endif // COMMUNICATE_H
