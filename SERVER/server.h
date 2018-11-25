#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTableView>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QVector>
#include <QMap>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSet>
#include <QDebug>
#include <QDataStream>
#include <QString>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include "dealuserinfo.h"
#include "communicate.h"
#include "sockets.h"

namespace Ui {
class SERVER;
}

class SERVER : public QWidget
{
    Q_OBJECT
public:
    explicit SERVER(QWidget *parent = 0);
    ~SERVER();
    void Init();
    //初始化服务端储存信息和界面菜单
    void ShowUser(QString userName,int row,
                  const QString& tcpPort,
                  const QString& udpPort);
    //用于显示用户信息
    int AssignPort();
    //为上线的用户随机分配TCP和UDP端口
private slots:
    void HasNewConnection();//监听到新连接
    void GetMsg();
    //从套接字获取用户信息并创建新对象分析处理信息
    void DisConnection();
    //用户断开和服务端的连接,在服务端显示窗口删除用户信息
private:
    QTcpServer *myServer;
    QStandardItemModel *menu;
public:
    Ui::SERVER *ui;
    QVector<SocketInfo*> socketList;
    //管理服务端与各用户之间的socket
    QMap<QString,UserInfo*> userInfoList;
    //存所有用户的信息
    QSet<int> ports;//存放给在线用户分配的端口
    QVector<OffMsg> offMsgList;//存放离线消息
};

#endif // SERVER_H
