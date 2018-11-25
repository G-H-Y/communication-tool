#include "server.h"
#include "communicate.h"
#include "ui_server.h"
#include <QAction>
#include <QHeaderView>
#include <QByteArray>

SERVER::SERVER(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SERVER)
{
    ui->setupUi(this);
    this->Init();  //初始化信息
    myServer = new QTcpServer(this);
    myServer->listen(QHostAddress::AnyIPv4,8888);  //开始监听
    connect(myServer,SIGNAL(newConnection()),this,SLOT(HasNewConnection()));
}

SERVER::~SERVER()
{
    myServer->close();
    delete ui;
    delete myServer;
    delete menu;
}

//初始化
void SERVER::Init()
{
    DEALUSERINFO::GetOffMsg(offMsgList);
    DEALUSERINFO::GetUserInfo(userInfoList);

    this->menu=new QStandardItemModel();
    menu->setColumnCount(3);
    menu->setHorizontalHeaderItem(0,new QStandardItem("IP"));
    menu->setHorizontalHeaderItem(1,new QStandardItem("USER"));
    menu->setHorizontalHeaderItem(2,new QStandardItem("TCP"));
    menu->setHorizontalHeaderItem(3,new QStandardItem("UDP"));
    ui->tableView->horizontalHeader()->setDefaultSectionSize(148);
    ui->tableView->setModel(menu);
    ui->tableView->horizontalHeader()->show();
}

void SERVER::GetMsg()
{
    for(SocketInfo* & skt : socketList)
    {

        QByteArray getArray=skt->socket->readAll();   //从套接字中获取用户消息
        if(!getArray.isEmpty())
        {
            QJsonParseError pErr;
            QJsonDocument getDoc(QJsonDocument::fromJson(getArray,&pErr));//将用户消息转换成json文件
            if(pErr.error==QJsonParseError::NoError){
                QJsonObject tmpobj=getDoc.object();
                if(tmpobj.contains("request")){
                    COMMUNICATE communicate(this,tmpobj,skt); //创建一个对象处理用户消息
                }
            }
        }
    }
}

void SERVER::ShowUser(QString userName, int row, const QString &tcpPort, const QString &udpPort){
    menu->setItem(row,1,new QStandardItem(userName));
    menu->setItem(row,2,new QStandardItem(tcpPort));
    menu->setItem(row,3,new QStandardItem(udpPort));
}

int SERVER::AssignPort()
{
    qsrand(0);
    int i = 15000;
    while(i--){
        int port=qrand()%25000+80;
        if(ports.contains(port))
            continue;
        ports.insert(port);
        return port;
    }
    return -1;
}

void SERVER::HasNewConnection(){
    QTcpSocket *socket=myServer->nextPendingConnection();
    menu->setItem(socketList.size(),0,new QStandardItem(socket->peerAddress().toString()));
    SocketInfo *sktInfo= new SocketInfo(socket,socketList.size());
    socketList.push_back(sktInfo);

    connect(socket,SIGNAL(readyRead()),this,SLOT(GetMsg()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(DisConnection()));
}

void SERVER::DisConnection()
{
   for(int s=0; s<socketList.size(); s++)
   {
       SocketInfo *sktInfo = socketList[s];
       QTcpSocket *skt = sktInfo->socket;
       if(skt->state()!=QAbstractSocket::UnconnectedState)
         s++;
       else
       {
           menu->removeRow(sktInfo->row);
           for(int l=s+1; l<socketList.size(); l++)
               --socketList[l]->row;
           socketList.remove(s);
       }
   }
}
