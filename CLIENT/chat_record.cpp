#include "chat_record.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QDebug>
#define FILEPATH "F:\\"
#define FILENAME "chat.txt"

void CHAT_RECORD::GetChatRecord(const QString &userName, QMap<QString, USERITEM *> usersMsg){
    QFile file(FILEPATH+userName+FILENAME);
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray getBytes=file.readAll();
        QJsonDocument getDoc=QJsonDocument::fromJson(getBytes);
        QJsonObject getObj=getDoc.object();
        QJsonArray getArr=getObj["chatRecord"].toArray();
        for(int i=0; i<getArr.size(); ++i){
            QJsonObject tmpObj=getArr[i].toObject();
            QJsonArray msgArr=tmpObj["chatRecord"].toArray();
            QString user=tmpObj["user"].toString();
            usersMsg[user]->msgNum=tmpObj["messageNum"].toInt();
            for(int j=0; j<msgArr.size();++j){
                QJsonObject msgObj=msgArr[j].toObject();
                int rev_send = msgObj["rev_send"].toInt();
                QString message = msgObj["message"].toString();
                MESSAGE msg(rev_send,message);
                usersMsg[user]->messages.push_back(msg);
            }
        }
        file.close();
    }
}

void CHAT_RECORD::SaveChatRecord(const QString &userName, QMap<QString, USERITEM *> usersMsg){
    QFile file(FILEPATH+userName+FILENAME);
    if(file.open(QIODevice::WriteOnly))
    {
        QJsonObject saveObj;
        QJsonArray saveArr;
        QJsonDocument saveDoc(saveObj);
        QMap<QString,USERITEM*>::iterator itr = usersMsg.begin();
        while(itr!=usersMsg.end()){
            QJsonObject tmpObj;
            QJsonArray msgArr;
            USERITEM *tmpItem = itr.value();
            for(MESSAGE &msg : tmpItem->messages){
                QJsonObject msgObj;
                msgObj["rev_send"]=msg.rev_send;
                msgObj["message"]=msg.message;
                msgArr.append(msgObj);
            }
            tmpObj["messageNum"]=tmpItem->msgNum;
            tmpObj["chatRecord"]=msgArr;
            tmpObj["user"]=tmpItem->user;
            saveArr.append(tmpObj);
            ++itr;
        }
        saveObj["chatRecord"]=saveArr;
        file.write(saveDoc.toJson());
        file.close();
    }
}
