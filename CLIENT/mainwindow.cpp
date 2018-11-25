#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QThreadPool>
#include <QLabel>
#include <QtConcurrent>
#include <QDateTime>
#include "chat_record.h"
#include "communicate.h"
#include "signinview.h"
#include "findpsdview.h"
#include "signupview.h"
#include "linkserverview.h"

//初始化界面并连接服务器
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->InitInterface();
    this->ConnectServer("127.0.0.1","8888");
}

//析构函数
MainWindow::~MainWindow()
{
    delete userListView;
    delete clientServer;
    delete udpSocket;
    delete connectServerSocket;
    delete ui;
}

//初始化主界面
void MainWindow::InitInterface(){
    userListView=new SHOWUSERLIST(ui->centralWidget);
    userListView->setGeometry(0,0,291,611);
    //在用户列表中点击某用户
    connect(userListView,SIGNAL(clicked(QModelIndex)),this,SLOT(ChooseUser(QModelIndex)));
    ui->sendMsg->setEnabled(false);

    QAction *aSignIn=new QAction("SIGN_IN",this);
    connect(aSignIn,SIGNAL(triggered(bool)),this,SLOT(SignInSlot()));
    ui->usermenu->addAction(aSignIn);

    QAction *aSignUp=new QAction("SIGN_UP",this);
    connect(aSignUp,SIGNAL(triggered(bool)),this,SLOT(SignUpSlot()));
    ui->usermenu->addAction(aSignUp);

    QAction *aFindPsd=new QAction("FIND_PSD",this);
    connect(aFindPsd,SIGNAL(triggered(bool)),this,SLOT(FindPsdSlot()));
    ui->usermenu->addAction(aFindPsd);

    QAction *aLink=new QAction("LINK",this);
    connect(aLink,SIGNAL(triggered(bool)),this,SLOT(ConnectServerSlot()));
    ui->servermenu->addAction(aLink);
}

//点击选择用户通话
void MainWindow::ChooseUser(QModelIndex index){
    QString user=userListView->showUserList[index.row()].userName->text();
    userListView->showUserList[index.row()].message->hide();
    this->chatUser=user;
    USERITEM *userMsg=msgRecord[user];
    userMsg->msgNum=0;
    ui->listWidget->clear();
    for(MESSAGE msg : userMsg->messages)
        MainWindow::ShowMsg(msg);

    ui->listWidget->verticalScrollBar()->setValue(ui->listWidget->verticalScrollBar()->maximum());
    //如果还没有和该用户建立连接，则向服务器请求该用户是否在线
    if(userMsg->socket==nullptr&&userMsg->isConnected!=true){
      QJsonObject sendObj;
      sendObj["request"]=COMMUNICATE::LINK_FRIEND;
      sendObj["friend"]=user;
      COMMUNICATE::Send(connectServerSocket,sendObj);
    }
}

//设置一条消息的显示方式
void MainWindow::ShowMsg(MESSAGE message){
    QWidget wgt;
    QHBoxLayout lyt(&wgt);
    QLabel lbl(message.message);
    QListWidgetItem item(ui->listWidget);

    if(message.rev_send==RECEIVER){
        lbl.setMinimumWidth(100);
        lbl.setMargin(40);
        lbl.setStyleSheet("QLabel{border-image:url(:/icon/icon/chat3.png);font-size:16}");
        lyt.setAlignment(Qt::AlignLeft);
    }

    else{
        lbl.setMinimumWidth(100);
        lbl.setMargin(40);
        lbl.setStyleSheet("QLabel{border-image:url(:/icon/icon/chat1.png)}");
        lyt.setAlignment(Qt::AlignRight);
    }

    lyt.addWidget(&lbl);
    wgt.setLayout(&lyt);
    ui->listWidget->addItem(&item);
    ui->listWidget->setItemWidget(&item,&wgt);
    item.setSizeHint(QSize(0,80));
    wgt.show();

    ui->listWidget->verticalScrollBar()->setValue(ui->listWidget->verticalScrollBar()->maximum());
}

//从套接字中读取消息并且分析  主动发送tcp请求的socket
void MainWindow::GetMsg(){
    QMap<QString,USERITEM*>::iterator itr=msgRecord.begin();
    while(itr!=msgRecord.end()){
        QTcpSocket *socket=itr.value()->socket;
        if(socket!=nullptr&&socket->isReadable()&&socket->bytesAvailable()){
            QByteArray getArr=socket->readAll();
            QJsonParseError *pErr;
            QJsonDocument getDoc(QJsonDocument::fromJson(getArr,pErr));
            if(pErr->error==QJsonParseError::NoError){
                QJsonObject getObj=getDoc.object();
                if(getObj.contains("request")){
                    COMMUNICATE communicate(this,getObj);
                }
            }
        }
        ++itr;
    }
}

//获取好友udp端口  接受其他客户端连接的socket
//响应其他客户端发来的tcp连接请求之后，向对方好友发送请求获取对方好友信息，更新userMsg中对方好友的状态和信息
void MainWindow::GetMsgFromUser(){
    QMap<int,QTcpSocket*>::iterator itr=users.begin();
    while(itr!=users.end()){
            QTcpSocket *socket=itr.value();
            if(socket!=nullptr&&socket->isReadable()&&socket->bytesAvailable()){
                QByteArray getArr=socket->readAll();
                QJsonParseError *pErr;
                QJsonDocument getDoc(QJsonDocument::fromJson(getArr,pErr));
                if(pErr->error==QJsonParseError::NoError){
                    QJsonObject tmpObj=getDoc.object();
                    QString username = tmpObj["username"].toString();
                    USERITEM *userMsg = msgRecord[username];
                    userMsg->socket=itr.value();
                    userMsg->udpPort=tmpObj["udpPort"].toString().toInt();
                    userMsg->isConnected=true;
                    connect(userMsg->socket,SIGNAL(readyRead()),this,SLOT(GetMsg()));
                    connect(userMsg->socket,SIGNAL(disconnected()),this,SLOT(DisconnectServer()));
                    users.remove(itr.key());  //在users中删除该套接字
                }
            }
        ++itr;
    }
}

//将和其他用户的socket放在一个vector中存着
//响应其他客户端发来的tcp连接请求
void MainWindow::HasNewConnection(){
    QTcpSocket *socket=clientServer->nextPendingConnection();
    int tcpPort = socket->peerPort();
    users[tcpPort]=socket;

    QJsonObject req;  //和用户建立tcp连接后，向用户请求得到该用户的udp端口
    req["request"]=COMMUNICATE::GET_FRDINFO;
    req["username"]=userState.userName;
    COMMUNICATE::Send(socket,req);
    connect(socket,SIGNAL(readyRead()),this,SLOT(GetMsgFromUser()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(DisconnectServer()));
}

//登录
void MainWindow::SignIn(const QString &userName, const QString &password){
    QJsonObject signInObj;
    signInObj["request"]=COMMUNICATE::SIGN_IN;
    signInObj["username"]=userName;
    signInObj["password"]=password;
    COMMUNICATE::Send(connectServerSocket,signInObj);
}

//注册
void MainWindow::SignUp(const QString &userName, const QString &password, const QString &answer){
    QJsonObject signUpObj;
    signUpObj["request"]=COMMUNICATE::SIGN_UP;
    signUpObj["username"]=userName;
    signUpObj["password"]=password;
    signUpObj["answer"]=answer;
    COMMUNICATE::Send(connectServerSocket,signUpObj);
}

//找回密码
void MainWindow::FindPsd(const QString &userName, const QString &answer){
    QJsonObject fndPsdObj;
    fndPsdObj["request"]=COMMUNICATE::FIND_PSD;
    fndPsdObj["username"]=userName;
    fndPsdObj["answer"]=answer;
    COMMUNICATE::Send(connectServerSocket,fndPsdObj);
}

//退出登录
void MainWindow::SignOutSlot(){
    QJsonObject signOutObj;
    QString username = userState.userName;
    signOutObj["request"]=COMMUNICATE::SIGN_OUT;
    signOutObj["username"]=username;
    if(username.length())
        CHAT_RECORD::SaveChatRecord(username,msgRecord);
    if(clientServer)
    {
        clientServer->close();
        clientServer->deleteLater();
        clientServer = nullptr;
    }

    QMap<int,QTcpSocket*>::iterator usersItr=users.begin();
    while(usersItr!=users.end()){
        CloseSocket(usersItr.value());
    }

    QMap<QString,USERITEM*>::iterator userMsgItr=msgRecord.begin();
    for(;userMsgItr!=msgRecord.end();++userMsgItr){
        QTcpSocket *socket=userMsgItr.value()->socket;
        CloseSocket(socket);
        userMsgItr.value()->isConnected=false;
    }

    CloseSocket(udpSocket);

    userState.isSignIn = false;
    userState.userName = "";
    chatUser = "";
    users.clear();
    msgRecord.clear();

    userListView->clear();
    ui->listWidget->clear();
    QAction *remv=ui->usermenu->actions().at(0);
    ui->usermenu->removeAction(remv);
    QAction *fstActn=ui->usermenu->actions().at(0);
    QAction *signInActn=new QAction("SIGN_IN",this);
    connect(signInActn,SIGNAL(triggered(bool)),this,SLOT(SignInSlot()));
    ui->usermenu->insertAction(fstActn,signInActn);
    remv=ui->usermenu->actions().at(3);
    ui->usermenu->removeAction(remv);
}

void MainWindow::CloseSocket(QTcpSocket *skt)
{
    if(skt!=nullptr)
    {
        skt->close();
        skt->deleteLater();
        skt = nullptr;
    }
}
void MainWindow::CloseSocket(QUdpSocket *skt)
{
    if(skt!=nullptr)
    {
        skt->close();
        skt->deleteLater();
        skt = nullptr;
    }
}

//获取从服务器发来的消息
void MainWindow::GetMsgFromServer(){
    if(connectServerSocket->isReadable()&&connectServerSocket->bytesAvailable()){
        QByteArray getArr=connectServerSocket->readAll();
        QJsonParseError *pErr;
        QJsonDocument getDoc=(QJsonDocument::fromJson(getArr,pErr));

        if(pErr->error==QJsonParseError::NoError&&getDoc.isObject()){
            QJsonObject getObj=getDoc.object();
            if(getObj.contains("request")){
                COMMUNICATE commun(this,getObj);
            }
        }
    }
}

//连接服务器
void MainWindow::ConnectServer(const QString &ip, const QString &port){
    connectServerSocket=new QTcpSocket;
    connectServerSocket->connectToHost(ip,port.toInt());
    connect(connectServerSocket,SIGNAL(readyRead()),this,SLOT(GetMsgFromServer()));
    connect(connectServerSocket,SIGNAL(connected()),this,SLOT(ConnectServSucceed()));
    connect(connectServerSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ConnectServErr(QAbstractSocket::SocketError)));
    connect(connectServerSocket,SIGNAL(disconnected()),this,SLOT(DisconnectServer()));
}

//与服务器连接成功
void MainWindow::ConnectServSucceed(){
    userState.isServer =true;
    QMessageBox::information(this,"HINT","Connect server successfully",QMessageBox::Yes);
}

//与服务器连接失败
void MainWindow::ConnectServErr(QAbstractSocket::SocketError error){
    userState.isServer=false;
    userState.isSignIn=false;
     QMessageBox::information(this,"HINT","Fail to connect server",QMessageBox::Yes);
}

//与服务器断开连接
void MainWindow::DisconnectServer(){
    if(connectServerSocket->state()==QAbstractSocket::UnconnectedState)
        CloseSocket(connectServerSocket);
    QString username = userState.userName;
    if(username.length())
        CHAT_RECORD::SaveChatRecord(username,msgRecord);
    if(clientServer)
    {
        clientServer->close();
        clientServer->deleteLater();
        clientServer = nullptr;
    }

    QMap<int,QTcpSocket*>::iterator usersItr=users.begin();
    while(usersItr!=users.end()){
        CloseSocket(usersItr.value());
    }

    QMap<QString,USERITEM*>::iterator userMsgItr=msgRecord.begin();
    for(;userMsgItr!=msgRecord.end();++userMsgItr){
        QTcpSocket *socket=userMsgItr.value()->socket;
        CloseSocket(socket);
        userMsgItr.value()->isConnected=false;
    }

    CloseSocket(udpSocket);

    userState.isSignIn = false;
    userState.userName = "";
    chatUser = "";
    users.clear();
    msgRecord.clear();

    userListView->clear();
    ui->listWidget->clear();
    QAction *remv=ui->usermenu->actions().at(0);
    ui->usermenu->removeAction(remv);
    QAction *fstActn=ui->usermenu->actions().at(0);
    QAction *signInActn=new QAction("SIGN_IN",this);
    connect(signInActn,SIGNAL(triggered(bool)),this,SLOT(SignInSlot()));
    ui->usermenu->insertAction(fstActn,signInActn);
    remv=ui->usermenu->actions().at(3);
    ui->usermenu->removeAction(remv);
}

//显示登录界面
void MainWindow::SignInSlot(){
    SIGNINVIEW *signIn=new SIGNINVIEW(this);
    signIn->setWindowTitle("SIGN_IN");
    signIn->show();
}

//显示找回密码界面
void MainWindow::FindPsdSlot(){
    FINDPSDVIEW *fndPsd=new FINDPSDVIEW(this);
    fndPsd->show();
}

//显示注册界面
void MainWindow::SignUpSlot(){
    SIGNUPVIEW *signUp=new SIGNUPVIEW(this);
    signUp->show();
}

//显示连接服务器界面
void MainWindow::ConnectServerSlot(){
    LINKSERVERVIEW *linkSev=new LINKSERVERVIEW(this);
    linkSev->show();
}

//有消息输入
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
   if(ui->lineEdit->text().length()>0)
       ui->sendMsg->setEnabled(true);
   else ui->sendMsg->setEnabled(false);
}

//点击发送消息按钮
void MainWindow::on_sendMsg_clicked()
{
   if(chatUser!="")
   {
       USERITEM *userMsg=msgRecord[chatUser];
       QJsonObject chatObj;
       QString message=ui->lineEdit->text();
       chatObj["sender"]=this->userState.userName;
       chatObj["request"]=COMMUNICATE::SEND_MSG;
       chatObj["receiver"]=userMsg->user;
       chatObj["message"]=message;

       MESSAGE msg(SENDER,message);
       userMsg->messages.push_back(msg);
       this->ShowMsg(msg);

       if(userMsg->isConnected){
           COMMUNICATE::Send(userMsg->socket,chatObj);
       }
       else{
           COMMUNICATE::Send(connectServerSocket,chatObj);
       }
       ui->lineEdit->setText("");
   }
   else
       QMessageBox::information(this,"HINT","Please choose a user",QMessageBox::Yes,QMessageBox::No);
}

//发送文件
void MainWindow::on_openFile_clicked()
{
    if(chatUser==""){
        QMessageBox::information(this,"HINT","Please choose a user",QMessageBox::Yes,QMessageBox::No);
        return;
    }

    if(!msgRecord[chatUser]->isConnected||msgRecord[chatUser]->socket==nullptr){
        QMessageBox::information(this,"HINT","The user is not online",QMessageBox::Yes,QMessageBox::No);
        return;
    }
    int udpPort = msgRecord[chatUser]->udpPort;
    if(sendFileList.contains(udpPort)){
        QMessageBox::information(this,"HINT","Send one file at one time",QMessageBox::Yes);
        return;
    }

    QString fPath=QFileDialog::getOpenFileName(this,"Choose File","F:\\");
    if(fPath.length())
    {
        QFile *file = new QFile(fPath);
        if(file->open(QIODevice::ReadOnly))
        {
            QFileInfo fileInfo(*file);
            RECEIVEFILE *revFile=new RECEIVEFILE;
            sendFileList[udpPort]=revFile;
            revFile->file=file;
            revFile->fileSize=file->size();
            revFile->progress=new PROGRESSBARVIEW(this);
            revFile->progress->SetText("send file"+fileInfo.fileName()+"to"+chatUser);
            revFile->progress->show();

            QHostAddress *hstAddr=new QHostAddress(msgRecord[chatUser]->socket->peerAddress());
            revFile->udpPort = udpPort;
            revFile->udpSocket = udpSocket;
            revFile->sender = *hstAddr;
            QByteArray *sendBytes=new QByteArray;
            QDataStream dataStm(sendBytes,QIODevice::WriteOnly);
            dataStm<<COMMUNICATE::NEW_FILE<<fileInfo.fileName()<<file->size();

            delete revFile->sendBytes;
            revFile->sendBytes = sendBytes;

            udpSocket->writeDatagram(*sendBytes,*hstAddr,udpPort);
            revFile->time.start(TIMEOUT);
        }
    }
}

void MainWindow::SendRevFile()
{
    int condition;
    int packetId;
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress sendAddr;
        quint16 sendPort;
        datagram.resize(udpSocket->pendingDatagramSize());
        char* readdata = datagram.data();
        qint64 datasize = datagram.size();
        udpSocket->readDatagram(readdata,datasize,&sendAddr,&sendPort);
        QDataStream dataStm(&datagram,QIODevice::ReadOnly);
        dataStm>>condition;

        switch(condition)
        {
        //刚开始接收到文件
        case COMMUNICATE::NEW_FILE:
        {
            QByteArray sendBytes;
            QDataStream sendDataStm(&sendBytes,QIODevice::WriteOnly);
            if(!recvFileList.contains(sendPort))
            {
                RECEIVEFILE *revFile=new RECEIVEFILE;
                QString fileName;
                dataStm>>fileName>>revFile->fileSize;  //读取传送文件的文件名和文件大小
                revFile->file=new QFile(fileName);  //新建一个文件

                if(revFile->file->open(QIODevice::WriteOnly))
                {
                    recvFileList[sendPort]=revFile;
                    revFile->progress=new PROGRESSBARVIEW(this);
                    revFile->progress->SetText("Receiving"+fileName);
                    revFile->progress->show();
                }
            }
            sendDataStm<<COMMUNICATE::SEND_FILE<<0;
            udpSocket->writeDatagram(sendBytes,sendAddr,sendPort);
            break;
        }

          //如果接收方接收最后一个窗口的文件
        case COMMUNICATE::END_SEND_FILE:
        {
            if(sendFileList.contains(sendPort))
            {
                RECEIVEFILE *revFl=sendFileList[sendPort];
                sendFileList.remove(sendPort); //传输完毕后在发送列表中删除当前udpsocket
                revFl->time.stop();
                revFl->progress->close();
                QtConcurrent::run([revFl]{
                    revFl->file->close();  //关闭udpsocket的同时关闭文件
                    delete revFl;  //同时删除发送列表中相应的对象
                });
            }
            break;
        }

//如果正在传输文件
        case COMMUNICATE::SEND_FILE:
        {
            RECEIVEFILE *sendFile=sendFileList[sendPort];
            dataStm>>packetId; //读取接收方发送过来的当前已经接收到的包ID

            if(packetId==sendFile->packId)
            {
                //如果接受方已经接受到当前窗口的所有包
                sendFile->sndRevSize+=SEG*CACHE;  //更新已经发送的文件大小
                sendFile->progress->SetProgress(sendFile->sndRevSize*100/sendFile->fileSize);
                sendFile->sendCache.clear();
                sendFile->time.stop(); //关闭上一个窗口传输计时器，停止计时
                for(int i=0;i<CACHE;i++)
                {  //分割剩余文件并发送
                    QByteArray sndBytes;
                    QDataStream sendDtStm(&sndBytes,QIODevice::WriteOnly);
                    if(sendFile->file->atEnd())
                    {
                        sendDtStm<<COMMUNICATE::END_RECV_FILE;
                        sendFile->sendCache.insert((sendFile->packId+i)%CACHE,&sndBytes);
                    }
                    else
                    {
                        QByteArray readData=sendFile->file->read(SEG);
                        sendDtStm<<COMMUNICATE::RECV_FILE<<sendFile->packId+i<<readData;
                        sendFile->sendCache.insert((sendFile->packId+i)%CACHE,&sndBytes);
                    }
                    udpSocket->writeDatagram(sndBytes,sendAddr,sendPort);
                }
                sendFile->packId+=CACHE;
                sendFile->time.start();//开始计时
            }
            break;
        }

//发送方已经发送完全部文件内容
       case COMMUNICATE::END_RECV_FILE:
        {
            QByteArray rlyBytes;
            QDataStream sndDs(&rlyBytes,QIODevice::WriteOnly);
            if(recvFileList.contains(sendPort))
            {
                RECEIVEFILE *revFile=recvFileList[sendPort];
                revFile->progress->close();
                recvFileList.remove(sendPort);

                QtConcurrent::run(&threadPool,[revFile]{
                    QMap<int,QByteArray*>::iterator itr=revFile->revCache.begin();
                    while(itr!=revFile->revCache.end())
                    {
                        revFile->file->write(*itr.value());
                        revFile->sndRevSize += itr.value()->size();
                    }
                    revFile->file->close();
                    delete revFile;
                });
            }
            sndDs<<COMMUNICATE::END_SEND_FILE;
            udpSocket->writeDatagram(rlyBytes,sendAddr,sendPort);
            break;
        }

//正在接收文件
       case COMMUNICATE::RECV_FILE:
        {
            dataStm>>packetId;
            RECEIVEFILE *revF=recvFileList[sendPort];
            if(revF->packId <= packetId && packetId <= revF->packId+CACHE-1 && !revF->revCache.contains(packetId))
            {
                QByteArray fileArr;
                dataStm>>fileArr; //读取数据包中的正式文件内容
                revF->revCache.insert(packetId%CACHE,&fileArr);

                //如果已经收齐了当前窗口全部的包
                if(revF->revCache.size()>=CACHE){//将收到的包存入文件
                    QMap<int,QByteArray*>::iterator itr=revF->revCache.begin();
                    while(itr!=revF->revCache.end())
                    {
                        revF->file->write(*itr.value());
                        revF->sndRevSize += itr.value()->size();
                    }
                    revF->progress->SetProgress(revF->sndRevSize*100/revF->fileSize);
                    revF->revCache.clear();
                    revF->packId+=CACHE;  //更新已经收到的包的ID

                    QByteArray sdBytes;
                    QDataStream sndDs(&sdBytes,QIODevice::WriteOnly);
                    sndDs<<COMMUNICATE::SEND_FILE<<revF->packId;  //向发送方确认已经收到的包的ID
                    udpSocket->writeDatagram(sdBytes,sendAddr,sendPort);
                }
            }
            break;
        }

        default:
            break;
    }
}
}
