#include "sockets.h"

SocketInfo::SocketInfo(QTcpSocket *socket, int row):row(row),socket(socket){}
