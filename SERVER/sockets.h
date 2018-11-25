#ifndef SOCKETS_H
#define SOCKETS_H
#include <QTcpSocket>
class SocketInfo
{
public:
    QTcpSocket *socket;
    QString userName="";
    int row;
    int tcpPort;
    int udpPort;
    SocketInfo(QTcpSocket *socket,int row);
};
#endif // SOCKETS_H
