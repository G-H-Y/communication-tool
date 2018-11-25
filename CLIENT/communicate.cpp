#include <QThread>
#include <QDebug>
#include <QMessageBox>
#include "timetrack.h"
#include "log.h"
#include "communicate.h"
#include "ui_mainwindow.h"
#include "chat_record.h"

COMMUNICATE::COMMUNICATE(MainWindow *mainWin, QJsonObject &object):mainWin(mainWin),object(object){
    switch (object["request"].toInt()){
    case COMMUNICATE::SIGN_IN:
        this->SignIn();
        break;
    case COMMUNICATE::SIGN_UP:
        this->SignUp();
        break;
    case COMMUNICATE::FIND_PSD:
        this->FindPsd();
        break;
    case COMMUNICATE::SEND_MSG:
        this->SendMsg();
        break;
    case COMMUNICATE::LINK_FRIEND:
        this->ConnectFrd();
        break;
    case COMMUNICATE::GET_FRDINFO:
        this->GetFrdInfo();
        break;
    case COMMUNICATE::ADD_FRIEND:
        this->AddFriend();
        break;
    default:
        break;
    }
}

//从TCP发送消息
void COMMUNICATE::Send(QTcpSocket *socket, QJsonObject &sendObj){
    QJsonDocument sendDoc;
    sendDoc.setObject(sendObj);
    QByteArray sendArr=sendDoc.toJson();
    QDataStream(&sendArr,QIODevice::ReadWrite);
    socket->write(sendArr);
    socket->flush();
}

//登录
void COMMUNICATE::SignIn(){
    switch (object["result"].toInt())
    {
      case COMMUNICATE::USER_NOEXIST:
           QMessageBox::information(mainWin,"WARNING","User does no exist",QMessageBox::Yes);
           break;
      case COMMUNICATE::PSD_WRONG:
           QMessageBox::information(mainWin,"WARNING","Password does not match",QMessageBox::Yes);
           break;
      case COMMUNICATE::UNKNOWM:
           QMessageBox::information(mainWin,"WARNING","Please try it again",QMessageBox::Yes);
           break;
      case COMMUNICATE::NO_ERROR:
    {
        QMessageBox::information(mainWin,"WARNING","Sign in succeed",QMessageBox::Yes);
        int tcpPort = object["tcpPort"].toInt();
        int udpPort = object["udpPort"].toInt();
        QString userName = object["username"].toString();
        QJsonArray friends = object["friends"].toArray();
        QJsonArray offMsgArr=object["offMessage"].toArray();
        //portBind
        emit SignInSucceed();

        mainWin->clientServer = new QTcpServer;
        mainWin->udpSocket = new QUdpSocket(mainWin);

        mainWin->clientServer->listen(QHostAddress::AnyIPv4,tcpPort);
        mainWin->udpSocket->bind(udpPort);

        connect(mainWin->clientServer,SIGNAL(newConnection()),mainWin,SLOT(HasNewConnection()));
        connect(mainWin->udpSocket,SIGNAL(readyRead()),mainWin,SLOT(ReadFile()));
        //signInSucceed
        //登录成功之后将菜单第一行改为当前登录用户的名字和头像
        mainWin->userState.userName = userName;
        QAction *remv = mainWin->ui->usermenu->actions().at(0);
        mainWin->ui->usermenu->removeAction(remv);
        QAction *fstActn = mainWin->ui->usermenu->actions().at(0);
        QAction *userActn = new QAction(QIcon(":/icon/icon/user.png"),userName,mainWin);
        mainWin->ui->usermenu->insertAction(fstActn, userActn);
        //在用户菜单加上一行退出登录菜单
        QAction *signOut = new QAction("SIGNOUT",mainWin);
        mainWin->ui->usermenu->addAction(signOut);
        connect(signOut,SIGNAL(triggered(bool)),mainWin,SLOT(SignOutSlot()));
        //InitUserList
        for(int i = 0; i < friends.size(); ++i)
        {
            QJsonObject tmp = friends[i].toObject();
            QString user = tmp["friends"].toString();
            USERITEM *userItem = new USERITEM;
            userItem->ip = "";
            userItem->user = user;
            userItem->rowIndex = mainWin->msgRecord.size();
            mainWin->msgRecord[user] = userItem;
            mainWin->userListView->SetUser(QString(":/icon/icon/head%1.png").arg(i%3+1),user);
        }
        CHAT_RECORD::GetChatRecord(mainWin->userState.userName,mainWin->msgRecord);

        for(int i=0; i<offMsgArr.size();++i){
            QJsonObject offMsgObj=offMsgArr[i].toObject();
            QString sender=offMsgObj["sender"].toString();
            QString msg = offMsgObj["message"].toString();
            mainWin->msgRecord[sender]->msgNum++;
            mainWin->msgRecord[sender]->messages.push_back(MESSAGE(RECEIVER,msg));
        }

        QMap<QString,USERITEM*>::iterator itr=mainWin->msgRecord.begin();
        while(itr!=mainWin->msgRecord.end()){
            if(itr.value()->msgNum>0){
                mainWin->userListView->ShowMsgNum(itr.value()->rowIndex,itr.value()->msgNum);
                ++itr;
            }
        }
        break;
    }
      default:
           break;
    }
}

//注册
void COMMUNICATE::SignUp(){
    int result=object["result"].toInt();
    if(result==COMMUNICATE::USER_EXIST){
        QMessageBox::information(mainWin,"WARNING","User exists",QMessageBox::Yes);
    }
    else if(result==COMMUNICATE::NO_ERROR){
        QMessageBox::information(mainWin,"WARNING","Sign up succeed",QMessageBox::Yes);
    }
}

//找回密码
void COMMUNICATE::FindPsd(){
    switch(object["result"].toInt())
    {
      case COMMUNICATE::USER_NOEXIST:
           QMessageBox::information(mainWin,"WARNING","User does not exist",QMessageBox::Yes);
           break;
      case COMMUNICATE::ANSWER_WRONG:
           QMessageBox::information(mainWin,"WARNING","Your answer is not correct",QMessageBox::Yes);
           break;
      case COMMUNICATE::NO_ERROR:
           QMessageBox::information(mainWin,"WARNING","Your password is:"+object["password"].toString(),QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
           break;
     default:
           break;
    }
}

//收到消息
void COMMUNICATE::SendMsg(){
    QString user=object["sender"].toString();
    USERITEM *userMsg=mainWin->msgRecord[user];

    MESSAGE msg(RECEIVER,object["message"].toString());
    userMsg->messages.push_back(msg);
    if(user==mainWin->chatUser){
        userMsg->msgNum=0;  //如果是当前通信用户发来的消息则不用显示未读消息条数
        mainWin->ShowMsg(msg);
    }
    else{
        userMsg->msgNum++;
        mainWin->userListView->ShowMsgNum(userMsg->rowIndex,userMsg->msgNum);
    }
}

//收到从服务器传来的用户是否在线回复
void COMMUNICATE::ConnectFrd(){
    QString frd=object["friend"].toString();
    USERITEM *userMsg=mainWin->msgRecord[frd];
    int result=object["result"].toInt();
    if(result==COMMUNICATE::NO_ERROR)
    {
        userMsg->udpPort=object["udpPort"].toInt();
        userMsg->socket=new QTcpSocket();
        QString ip = object["ip"].toString();
        int tcpPort = object["tcpPort"].toInt();
        userMsg->socket->connectToHost(ip,tcpPort);
        connect(userMsg->socket,SIGNAL(readyRead()),mainWin,SLOT(GetMsg()));
        connect(userMsg->socket,SIGNAL(disconnected()),mainWin,SLOT(DisconnectServer()));
        //mainWin->AddConct(userMsg->socket);
        userMsg->isConnected=true;
    }
}

//给用户发送udp端口
void COMMUNICATE::GetFrdInfo(){
    QString frd=object["username"].toString();
    QJsonObject response;
    USERITEM *userMsg = mainWin->msgRecord[frd];
    response["username"]=mainWin->userState.userName;
    response["udpPort"]=QString::number(mainWin->udpSocket->localPort());
    COMMUNICATE::Send(mainWin->msgRecord[frd]->socket,response);
}


//有新用户注册
void COMMUNICATE::AddFriend(){
    QString newUser = object["username"].toString();
    mainWin->userListView->SetUser(":/icon/icon/head1.png",newUser);
    USERITEM *userItem = new USERITEM;
    userItem->ip = "";
    userItem->user = newUser;
    userItem->rowIndex = mainWin->msgRecord.size();
    mainWin->msgRecord[newUser]= userItem;
}
