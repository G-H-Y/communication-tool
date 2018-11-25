#include "log.h"
#include <QFile>

LOG::LOG(){

}

void LOG::LogInformation(const QString &log){
    QFile file("F:\\loginfo.txt");
    if(!file.open(QIODevice::Append))
        return;
    file.write(log.toUtf8());
    file.close();
}

void LOG::LogError(const QString &log){
    QFile file("F:\\logerror.txt");
    if(!file.open(QIODevice::Append))
        return;
    file.write(log.toUtf8());
    file.close();
}
