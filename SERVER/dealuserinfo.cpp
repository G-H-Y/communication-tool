#include "dealuserinfo.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDataStream>
#include <QFile>
#include "encryption/qaeswrap.h"
#define ENCRPT "5678"

DEALUSERINFO::DEALUSERINFO(){

}

void DEALUSERINFO::UpdateOffMsg(QVector<OffMsg> &offMsgStore)
{
    QFile file("F:\\offmsglist.txt");
    if(file.open(QIODevice::WriteOnly))
    {
        QJsonArray saveArray;
        QJsonObject saveObject;
        QJsonDocument saveDoc;
        for(OffMsg& m : offMsgStore)
        {
            QJsonObject tmpobj;
            tmpobj["reveiver"]=m.receiver;
            tmpobj["sender"]=m.sender;
            tmpobj["message"]=m.message;
            saveArray.append(tmpobj); //将该离线信息对象加入array
        }
        saveObject["offMsg"]=saveArray;  //将离线信息数组存入QJsonObject对象的key值为offMsg的value中
        saveDoc.setObject(saveObject); //将该对象存入QJsonDocument文件中

        QAesWrap aes(ENCRPT,ENCRPT,QAesWrap::AES_256); //加密

        file.write(aes.encrypt(saveDoc.toJson(),QAesWrap::AES_CTR));
        file.close();
    }
}

void DEALUSERINFO::GetOffMsg(QVector<OffMsg> &offMsgList)
{
    QFile file("F:\\offmsglist.txt");
    if(file.open(QIODevice::ReadOnly))
    {
        QAesWrap aes(ENCRPT,ENCRPT,QAesWrap::AES_256); //解密
        QByteArray readBytes=aes.decrypt(file.readAll(),QAesWrap::AES_CTR);
        QJsonParseError pErr;
        QJsonDocument getDoc(QJsonDocument::fromJson(readBytes,&pErr));

        if(pErr.error==QJsonParseError::NoError){
            //如果parsing成功
            QJsonArray getArray=getDoc.object()["offMsg"].toArray();
            for(int ia=0;ia<getArray.size();ia++){
                QJsonObject tmpobj=getArray[ia].toObject();
                OffMsg tmpmsg={tmpobj["receiver"].toString(),
                               tmpobj["sender"].toString(),
                               tmpobj["message"].toString()};
                offMsgList.push_back(tmpmsg);
            }
        }
        file.close();
    }
}

void DEALUSERINFO::UpdateUserInfo(QMap<QString, UserInfo *> &UserInfoStore){
    QFile file("F:\\userinfo.txt");
    if(file.open(QIODevice::WriteOnly))
    {
        QJsonArray saveArray;
        QJsonObject saveObject;
        QJsonDocument saveDoc;
        QMap<QString, UserInfo*>::iterator itr;
        for(itr=UserInfoStore.begin();itr!=UserInfoStore.end();itr++){
            QJsonObject tmpobj;
            tmpobj["username"]=itr.value()->username;
            tmpobj["password"]=itr.value()->psd;
            tmpobj["answer"]=itr.value()->answer;
            saveArray.append(tmpobj);
        }
        saveObject["users"]=saveArray;
        saveDoc.setObject(saveObject);

        QAesWrap aes(ENCRPT,ENCRPT,QAesWrap::AES_256);
        file.write(aes.encrypt(saveDoc.toJson(),QAesWrap::AES_CTR));
        file.close();
    }
}

void DEALUSERINFO::GetUserInfo(QMap<QString, UserInfo *> &UserInfoList){
    QFile file("F:\\userinfo.txt");
    if(file.open(QIODevice::ReadOnly))
    {
        QAesWrap aes(ENCRPT,ENCRPT,QAesWrap::AES_256);
        QByteArray getBytes=aes.decrypt(file.readAll(),QAesWrap::AES_CTR);
        QJsonParseError pErr;
        QJsonDocument getDoc(QJsonDocument::fromJson(getBytes,&pErr));

        if(pErr.error==QJsonParseError::NoError){
            QJsonArray getArray=getDoc.object()["users"].toArray();
            for(int i=0; i<getArray.size();i++){
                QJsonObject tmpobj=getArray[i].toObject();
                UserInfo *tmpuser= new UserInfo;
                tmpuser->username=tmpobj["username"].toString();
                tmpuser->psd=tmpobj["password"].toString();
                tmpuser->answer=tmpobj["answer"].toString();
                UserInfoList[tmpuser->username]=tmpuser;
            }
        }
        file.close();
    }
}
