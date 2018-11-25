#ifndef COMMUNICATE_H
#define COMMUNICATE_H
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include "mainwindow.h"

class COMMUNICATE:public QObject{
    Q_OBJECT
public:
    enum Result{
        NO_ERROR,
        USER_EXIST,
        USER_NOEXIST,
        PSD_WRONG,
        ANSWER_WRONG,
        USER_OFFLINE,
        UNKNOWM,
    };
    enum Request{
        SIGN_IN,
        SIGN_UP,
        SIGN_OUT,
        FIND_PSD,
        SEND_MSG,
        LINK_FRIEND,
        ADD_FRIEND,
        GET_FRDINFO
    };
    enum File{
        NEW_FILE,
        RECV_FILE,
        END_RECV_FILE,
        END_SEND_FILE,
        REJ_FILE,
        SEND_FILE
    };
    MainWindow *mainWin;
    QJsonObject object;
    COMMUNICATE(MainWindow *mainWin,QJsonObject& object);
    void SignIn();
    void SignUp();
    void FindPsd();
    void SendMsg();
    void ConnectFrd();
    void AddFriend();
    void GetFrdInfo();
    static void Send(QTcpSocket *socket,QJsonObject& sendObj);
signals:
    SignInSucceed();
};
#endif // COMMUNICATE_H
