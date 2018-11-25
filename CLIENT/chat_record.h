#ifndef CHAT_RECORD_H
#define CHAT_RECORD_H
#include <QMap>
#include "useritem.h"
class CHAT_RECORD{
public:
    static void GetChatRecord(const QString& userName,QMap<QString,USERITEM*> usersMsg);
    static void SaveChatRecord(const QString& userName,QMap<QString,USERITEM*> usersMsg);
};

#endif // CHAT_RECORD_H
