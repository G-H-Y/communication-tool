#include <QHBoxLayout>
#include "showuserlist.h"

SHOWUSERLIST::SHOWUSERLIST(QWidget *parent):QListWidget(parent)
{

}
void SHOWUSERLIST::SetUser(QString userIcon, QString userName, QString message){
    QWidget *wgt = new QWidget;
    QHBoxLayout *lyt = new QHBoxLayout(wgt);
    ShowUser *showUser = new ShowUser;
    QPixmap pxmp=QPixmap(userIcon);
    showUser->userIcon = new QLabel;
    showUser->userName=new QLabel(userName);
    showUser->message=new QLabel(message);
    showUser->userIcon->setMaximumWidth(60);
    showUser->userIcon->setScaledContents(true);
    showUser->userIcon->setPixmap(pxmp);
    showUser->message->setMaximumWidth(54);
    showUser->message->setAlignment(Qt::AlignCenter);
    showUser->message->setStyleSheet("QLabel{border-image:url(:/icon/icon/newMsg.png)}");
    showUser->message->hide();
    lyt->addWidget(showUser->userIcon);
    lyt->addWidget(showUser->userName);
    lyt->addWidget(showUser->message);

    wgt->setLayout(lyt);
    QListWidgetItem *item = new QListWidgetItem(this);
    this->addItem(item);
    this->setItemWidget(item,wgt);
    item->setSizeHint(QSize(0,80));
    wgt->show();
    showUserList.push_back(*showUser);
}

void SHOWUSERLIST::ShowMsgNum(int rowIndex, int msgNum){
    showUserList[rowIndex].message->setText(QString::number(msgNum));
    showUserList[rowIndex].message->show();
}

void SHOWUSERLIST::Clear(){
    showUserList.clear();
    QListWidget::clear();
}

