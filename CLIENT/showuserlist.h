#ifndef SHOWUSERLIST_H
#define SHOWUSERLIST_H
#include <QObject>
#include <QListWidget>
#include <QLabel>
#include <QWidget>

struct ShowUser{
    QLabel *userIcon;
    QLabel *userName;
    QLabel *message;
};

class SHOWUSERLIST:public QListWidget
{
    Q_OBJECT
public:
    SHOWUSERLIST(QWidget *parent);
    void SetUser(QString userIcon,QString userName,
                 QString message="");
    //显示用户名头像和消息条数
    void ShowMsgNum(int rowIndex,int msgNum); //显示消息条数
    void Clear();
    QVector<ShowUser> showUserList;
};

#endif // SHOWUSERLIST_H
