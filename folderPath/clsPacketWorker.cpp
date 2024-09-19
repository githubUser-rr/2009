#include "clsPacketWorker.h"
#include "clsPacketOperation.h"

#include <QThread>
#include <QProcess>
#include <QString>
#include <QMutex>
#include <windows.h>



int clsPacketWorker::currentThread = 0; // Başlatma
QMutex clsPacketWorker::threadMutex; // Static mutex nesnesinin başlatılması
const int clsPacketWorker::totalCpu = []() {
    SYSTEM_INFO sys;
    GetSystemInfo(&sys);
    return sys.dwNumberOfProcessors;
}();



clsPacketWorker::clsPacketWorker(const QString& str)
    : path(str){

    qDebug() << "Packet Worker constructor !! " << currentThread;
}

clsPacketWorker::~clsPacketWorker() {
    //decrementThread();
    delete packet;
    packet = nullptr;
    qDebug() << "Packet Worker Destructor !! " << currentThread;
}


void clsPacketWorker::incrementThread(){
    threadMutex.lock();
    currentThread++;
    threadMutex.unlock();
    qDebug() << "currentThread arttırıldı ! " << currentThread;

}

void clsPacketWorker::decrementThread(){
    threadMutex.lock();
    if(currentThread>0){
        currentThread--;
    }
    threadMutex.unlock();
    qDebug() << "currentThread azaltıldı ! " << currentThread;
}

void clsPacketWorker::createPacket(){
    //qDebug() << "Create Packet" ;
    //int maxThread = QThread::idealThreadCount(); //bunu static const olarak tanımlayabiliriz.



    while(true){
        threadMutex.lock();
        int useRate = returnUsageRate();
        qDebug() << "Kullanım oranı : " << useRate;
        qDebug() << "Anlık calisan thread sayisi : " << currentThread;

        if((useRate >= 80) && (currentThread >= totalCpu)){

            threadMutex.unlock();
            qDebug() << path << " dosya thread bind için bekliyor.";
            QThread::msleep(300);
            useRate = returnUsageRate();
        }else{

            int nowCpu = currentThread % totalCpu;
            HANDLE tHandle = GetCurrentThread();
            DWORD_PTR affinity = 1 << nowCpu;

            if (SetThreadAffinityMask(tHandle, affinity) == 0) {
                qDebug() << path << " CPU affinity ayarlanamadı: " << GetLastError() ;
            } else {
                qDebug() << path << "Thread " << nowCpu << ".CPU baglandi" ;
            }
            threadMutex.unlock();
            break;
        }

    }
    incrementThread();

    packet = new clsPacketOperation(path);
    packet->packetCapture(0);
    //packet->printPacketInfo();
    //packet->printCsvFile(); //bunu aç sonra

    /*delete packet;
    packet = nullptr;*/

    decrementThread();

    emit createFinished();

}

int clsPacketWorker::returnUsageRate(){


    /*QProcess prcs;
    prcs.start("wmic cpu get loadpercentage");
    prcs.waitForFinished();

    QString output = prcs.readAllStandardOutput();
    QStringList outList = output.split('\n');
    qDebug() << output ;
    outList.removeAll("");

    if(outList.size()>1){
        bool ok;
        double load = outList[1].trimmed().toDouble(&ok);
        if(ok){
            qDebug() << "Yüzde : " << load;
        }else{
            qDebug() << "Yüzde : " << " 0.0";
        }
    }*/


    static ULARGE_INTEGER lastIdleTime, lastKernelTime, lastUserTime;
    FILETIME idleTime, kernelTime, userTime;

    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    ULARGE_INTEGER currentIdleTime, currentKernelTime, currentUserTime;
    currentIdleTime.LowPart = idleTime.dwLowDateTime;
    currentIdleTime.HighPart = idleTime.dwHighDateTime;
    currentKernelTime.LowPart = kernelTime.dwLowDateTime;
    currentKernelTime.HighPart = kernelTime.dwHighDateTime;
    currentUserTime.LowPart = userTime.dwLowDateTime;
    currentUserTime.HighPart = userTime.dwHighDateTime;

    ULONGLONG idle = currentIdleTime.QuadPart - lastIdleTime.QuadPart;
    ULONGLONG kernel = currentKernelTime.QuadPart - lastKernelTime.QuadPart;
    ULONGLONG user = currentUserTime.QuadPart - lastUserTime.QuadPart;

    lastIdleTime = currentIdleTime;
    lastKernelTime = currentKernelTime;
    lastUserTime = currentUserTime;

    //mantıksal çekirdek
    //SYSTEM_INFO sys;
    //GetSystemInfo(&sys);
    //qDebug() << "Total çekirdek sayısı : " << sys.dwNumberOfProcessors;


    return (1.0 - (static_cast<double>(idle) / (kernel + user + idle))) * 100.0;


    /*int nowCpu = currCpu % totolCpu;
    HANDLE tHandle = GetCurrentThread();
    DWORD_PTR affinity = 1 << nowCpu;

    if (SetThreadAffinityMask(tHandle, affinity) == 0) {
        std::cerr << "CPU affinity ayarlanamadı: " << GetLastError() << std::endl;
    } else {
        std::cout << "Thread " << nowCpu << ".CPU baglandi" << std::endl;
    }*/






}


