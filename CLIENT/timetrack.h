#ifndef TIMETRACK_H
#define TIMETRACK_H
#include <QString>
#include <QUdpSocket>
#include <QByteArray>
#include <QMutex>
#include <QFile>
#include <QTimer>
#include "progressbarview.h"
#define CACHE 6
#define TIMEOUT 60

class RECEIVEFILE:public QObject
{
    Q_OBJECT
public:
    RECEIVEFILE();
    quint16 udpPort;
    QUdpSocket* udpSocket;
    QByteArray *sendBytes;
    QHostAddress sender;
    QTimer time;
    QMutex mutex;
    QMap<int,QByteArray*> sendCache;
    QFile *file;
    long long fileSize;
    long long sndRevSize;
    int packId;
    PROGRESSBARVIEW *progress;
    QMap<int,QByteArray*> revCache;
public slots:
    void ReSend();
};

#endif // TIMETRACK_H
