#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QFile>
#include <QtNetwork>
#include <QThreadPool>
#include "showuserlist.h"
#include "showuserlist.h"
#include "useritem.h"
#define SEG 10240

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void InitInterface(); //初始化主界面
    void ConnectServer(const QString &ip,const QString &port); //连接服务器
    void SignIn(const QString& userName, const QString& password);
    void SignUp(const QString& userName, const QString& password, const QString& answer);
    void FindPsd(const QString& userName, const QString& answer);
    void ShowMsg(MESSAGE message);
    void CloseSocket(QTcpSocket *skt);
    void CloseSocket(QUdpSocket *skt);
    void ClearMw();
public slots:
    void ChooseUser(QModelIndex);
    void SignInSlot();
    void SignUpSlot();
    void SignOutSlot();
    void FindPsdSlot();
    void HasNewConnection();
    void GetMsg();
    void GetMsgFromUser();
    void DisconnectServer();
    void GetMsgFromServer();
    void ConnectServerSlot();
    void ConnectServSucceed();
    void ConnectServErr(QAbstractSocket::SocketError);
    void SendRevFile();  //处理接收和传输文件
public:
    Ui::MainWindow *ui;
    USER_STATE userState;  //当前用户名称以及连接状态
    SHOWUSERLIST *userListView;  //显示用户列表
     QString chatUser;   //当前聊天用户
    QTcpServer *clientServer;  //监听其他客户端的tcp连接
    QUdpSocket *udpSocket;     //传输文件端口
    QTcpSocket *connectServerSocket;  //与服务器连接的tcpsocket
    QMap<QString,USERITEM*> msgRecord; //QString代表其他通话的用户
    QMap<int,QTcpSocket*> users;
    //保留其他用户发来的tcp连接的套接字
    //并且还没有得到对方好友发送的udp端口信息
    QMap<int,RECEIVEFILE*> sendFileList; //发送文件
    QMap<int,RECEIVEFILE*> recvFileList; //收到文件
    QThreadPool threadPool;
private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void on_sendMsg_clicked();
    void on_openFile_clicked();
};

#endif // MAINWINDOW_H
