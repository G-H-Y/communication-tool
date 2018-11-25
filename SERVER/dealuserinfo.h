#ifndef DEALUSERINFO_H
#define DEALUSERINFO_H
#include <QString>
#include <QDebug>
#include <QMap>

typedef struct UserInfo{
    QString username;
    QString psd;
    QString answer;
}UserInfo;

typedef struct OffMsg{
    QString receiver;
    QString sender;
    QString message;
}OffMsg;

class DEALUSERINFO
{
public:
    DEALUSERINFO();
    static void UpdateOffMsg(QVector<OffMsg>& offMsgStore);
    //将离线消息存入本地
    static void GetOffMsg(QVector<OffMsg> &offMsgList);
    //服务端运行时从本地读取信息
    static void UpdateUserInfo(QMap<QString,UserInfo*> &UserInfoStore);
    //将所有用户信息存入本地
    static void GetUserInfo(QMap<QString,UserInfo*> &UserInfoList);
    //读取用户信息
};
#endif // DEALUSERINFO_H
