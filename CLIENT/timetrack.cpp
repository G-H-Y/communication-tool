#include "timetrack.h"

RECEIVEFILE::RECEIVEFILE():udpSocket(nullptr){
    connect(&time,SIGNAL(timeout()),this,SLOT(ReSend()));
}

void RECEIVEFILE::ReSend(){
        mutex.lock();
        QMap<int,QByteArray*>::iterator itr=sendCache.begin();
        while(itr!=sendCache.end()){
            udpSocket->writeDatagram(*(itr.value()),sender,udpPort);
            ++itr;
        }
        mutex.unlock();
}
