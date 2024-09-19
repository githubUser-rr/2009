#include "PacketWorker.h"
#include <QDebug>
#include "packetoperation.h"
#include <iostream>
#include <QThread>

PacketWorker::PacketWorker(const std::string &str) : path(str){
    //qDebug() << "Packet Worker Cons" << path ;

}

void PacketWorker::createPacket(){
    qDebug() << "Create Packet" ;
    int maxThread = QThread::idealThreadCount();
    qDebug() << "Max Thread : " <<maxThread;
    packetOperation* packet = new packetOperation(path);
    packet->packetCapture(0);
    //packet->printPacketInfo();
    packet->printCsvFile(); //bunu aç sonra
    //packet->printSessionMap();
    delete packet;
    packet = nullptr;

    emit createFinished();
}
