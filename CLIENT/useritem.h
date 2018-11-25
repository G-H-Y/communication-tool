#ifndef USERITEM_H
#define USERITEM_H
#include <QString>
#include <QTcpSocket>
#include <QFile>
#include <QList>
#include "progressbarview.h"
#include "timetrack.h"
#define RECEIVER 1
#define SENDER   0
struct MESSAGE{
    MESSAGE(int rev_send=SENDER, QString message=""){
        this->rev_send = rev_send;
        this->message = message;
    }
    int rev_send;   //表示当前用户是本条消息的发送者还是接收者
    QString message;
};

struct USERITEM{
    int rowIndex;
    int msgNum;    //消息条数
    bool isConnected;  //对方好友是否和当前mainwindow类的user连接
    QTcpSocket *socket;  //两个用户通信的套接字
    QString ip;     //对方好友的ip地址
    QString user;   //对方好友的用户名
    int udpPort;
    QVector<MESSAGE> messages;  //用来存放消息记录
};

struct USER_STATE{
    bool isServer;
    bool isSignIn;
    QString userName;
};

#endif // USERITEM_H
