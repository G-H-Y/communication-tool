#include "communicate.h"
#include <QThread>

COMMUNICATE::COMMUNICATE(SERVER *myServer, QJsonObject &object, SocketInfo *sktInfo)
                        :myServer(myServer),object(object),sktInfo(sktInfo)
{
    switch (object["request"].toInt()){
    case COMMUNICATE::SIGN_IN:
        this->SignIn();
        break;
    case COMMUNICATE::SIGN_UP:
        this->SignUp();
        break;
    case COMMUNICATE::SIGN_OUT:
        this->SignOut();
        break;
    case COMMUNICATE::FIND_PSD:
        this->FindPsd();
        break;
    case COMMUNICATE::SEND_MSG:
        this->SendMsg();
        break;
    case COMMUNICATE::LINK_FRIEND:
        this->LinkFriend();
        break;
    default:
        break;
    }
}

void COMMUNICATE::Send(QTcpSocket *socket, QJsonObject &sendObject){
    QJsonDocument sendDoc;
    sendDoc.setObject(sendObject);
    QByteArray sendBytes=sendDoc.toJson();
    QDataStream(&sendBytes,QIODevice::ReadWrite); //
    socket->write(sendBytes);
    socket->flush();
}

void COMMUNICATE::SendOffMsg(QJsonArray  &sendOffMsg){
    QString user=object["username"].toString();
    QVector<OffMsg> &tmplist=myServer->offMsgList;
    for(int i=0;i<tmplist.size();){
        if(user!=tmplist[i].receiver) i++;
        //若存放离线消息的链表里存放发给登录用户的离线消息
        else
        {
            QJsonObject friendObject;
            friendObject["sender"]=tmplist[i].sender;
            friendObject["message"]=tmplist[i].message;
            sendOffMsg.append(friendObject);
            tmplist.remove(i); //发送完该离线消息后从服务器端链表删除
        }
    }
}

void COMMUNICATE::InformFriend(const QString &userName, const QString &ip, int udpPort, int tcpPort){
    QJsonObject inform;
    inform["request"]=COMMUNICATE::LINK_FRIEND;
    inform["result"]=COMMUNICATE::NO_ERROR;
    inform["friend"]=userName;
    inform["ip"]=ip;
    inform["udpPort"]=udpPort;
    inform["tcpPort"]=tcpPort;
    for(SocketInfo* skt : myServer->socketList){
        if(skt->userName!="")
           COMMUNICATE::Send(skt->socket,inform);
    }
}

void COMMUNICATE::SignIn(){
    QJsonObject response;
    QString user=object["username"].toString();
    response["request"]=COMMUNICATE::SIGN_IN;
    if(!myServer->userInfoList.contains(user)){
        response["result"]=COMMUNICATE::USER_NOEXIST;
        COMMUNICATE::Send(sktInfo->socket,response);
        return;
    } //用户不存在

    if(myServer->userInfoList[user]->psd!=object["password"].toString()){
        response["result"]=COMMUNICATE::PSD_WRONG;
        COMMUNICATE::Send(sktInfo->socket,response);
        return;
    } //密码不正确

    for(SocketInfo* skt : myServer->socketList){
        if(skt->userName==user){
            response["result"]=COMMUNICATE::UNKNOWN;
            COMMUNICATE::Send(sktInfo->socket,response);
            return;
        }
    } //用户已经在线

    //给新上线的用户分配端口
    int tcpPort=myServer->AssignPort();
    int udpPort=myServer->AssignPort();
    if(tcpPort==-1||udpPort==-1){
        response["result"]=COMMUNICATE::UNKNOWN;
        COMMUNICATE::Send(sktInfo->socket,response);
        return;
    } //端口分配失败

    //在服务端显示登录用户消息
    myServer->ShowUser(object["username"].toString(),sktInfo->row,
                         QString::number(tcpPort),QString::number(udpPort));
    sktInfo->userName=object["username"].toString();
    sktInfo->tcpPort=tcpPort;
    sktInfo->udpPort=udpPort;
    response["username"]=user;
    response["tcpPort"]=tcpPort;
    response["udpPort"]=udpPort;
    response["result"]=COMMUNICATE::NO_ERROR;

    //将所有用户信息发给登录用户
    QJsonArray users;
    QMap<QString,UserInfo*>::iterator itr=myServer->userInfoList.begin();
    while(itr!=myServer->userInfoList.end()){
        if(itr.value()->username!=user)
        {
            QJsonObject tmp;
            tmp["friend"]=itr.value()->username;
            users.append(tmp);
            ++itr;
        }
        else
        {
            ++itr;
            continue;
        }
    }
    //发送离线消息
    QJsonArray OffMsgArray;
    SendOffMsg(OffMsgArray);
    response["friends"]=users;
    response["offMessage"]=OffMsgArray;
    response["requset"]=COMMUNICATE::SIGN_IN;
    Send(sktInfo->socket,response);

    //提示其他用户该用户已上线
    COMMUNICATE::InformFriend(object["username"].toString(),
            sktInfo->socket->peerAddress().toString(),udpPort,tcpPort);
}

void COMMUNICATE::SignUp(){
    QString userName=object["username"].toString();
    if(myServer->userInfoList.contains(userName)){
        QJsonObject response;
        response["request"]=COMMUNICATE::SIGN_UP;
        response["result"]=COMMUNICATE::USER_EXIST;
        COMMUNICATE::Send(sktInfo->socket,response);
        return;
    } //用户名已存在

    //将新注册的用户存入服务端存的用户信息链表里
    QString password=object["password"].toString();
    QString answer=object["answer"].toString();
    UserInfo *saveUser=new UserInfo;
    saveUser->username=userName;
    saveUser->psd=password;
    saveUser->answer=answer;
    myServer->userInfoList[userName]=saveUser;

    //发送注册成功消息
    QJsonObject response;
    response["request"]=COMMUNICATE::SIGN_UP;
    response["result"]=COMMUNICATE::NO_ERROR;
    COMMUNICATE::Send(sktInfo->socket,response);

    //给当前在线的每位用户发送新用户信息
    QJsonObject toOther;
    toOther["request"]=COMMUNICATE::ADD_FRIEND;
    toOther["result"]=COMMUNICATE::NO_ERROR;
    toOther["username"]=userName;
    for(SocketInfo* skt : myServer->socketList){
        if(skt->userName!="")
            COMMUNICATE::Send(skt->socket,toOther);
    }
}

void COMMUNICATE::SignOut(){
    QString user=object["username"].toString();
    for(SocketInfo* skt : myServer->socketList){
        if(skt->userName==user){
            skt->userName="";
            myServer->ShowUser("",skt->row,"","");
            break;
        }
    }
}

void COMMUNICATE::FindPsd(){
    QJsonObject response;
    response["request"]=COMMUNICATE::FIND_PSD;
    QString user=object["username"].toString();
    QString answer=object["answer"].toString();
    QMap<QString,UserInfo*> list = myServer->userInfoList;

    if(!list.contains(user)){
        response["result"]=COMMUNICATE::USER_NOEXIST;
        COMMUNICATE::Send(sktInfo->socket,response);
        return;
    }  //用户不存在

    if(list[user]->answer!=answer){
        response["result"]=COMMUNICATE::ANSWER_WRONG;
        COMMUNICATE::Send(sktInfo->socket,response);
        return;
    } //回答不正确

    response["result"]=COMMUNICATE::NO_ERROR;
    response["password"]=list[user]->psd;
    COMMUNICATE::Send(sktInfo->socket,response);
}

void COMMUNICATE::SendMsg(){
    OffMsg offmessage;
    offmessage.message=object["message"].toString();
    offmessage.receiver=object["receiver"].toString();
    offmessage.sender=object["sender"].toString();
    myServer->offMsgList.push_back(offmessage);
}

void COMMUNICATE::LinkFriend(){
    QJsonObject response;
    QString frd=object["friend"].toString();
    response["request"]=COMMUNICATE::LINK_FRIEND;
    response["friend"]=frd;

    for(SocketInfo *skt : myServer->socketList){
        if(skt->userName==frd){
            response["result"]=COMMUNICATE::NO_ERROR;
            response["ip"]=skt->socket->peerAddress().toString();
            response["tcpPort"]=skt->tcpPort;
            response["udpPort"]=skt->udpPort;
            COMMUNICATE::Send(sktInfo->socket,response);
            return;
        }

    response["result"]=COMMUNICATE::USER_OFFLINE;
    COMMUNICATE::Send(sktInfo->socket,response);
   }
}
