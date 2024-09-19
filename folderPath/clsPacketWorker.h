#ifndef CLSPACKETWORKER_H
#define CLSPACKETWORKER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include "clsPacketOperation.h"


class clsPacketWorker : public QObject
{
    Q_OBJECT
public:
    clsPacketWorker(const QString& str);
    ~clsPacketWorker();

    static void incrementThread();
    static void decrementThread();

    static QMutex threadMutex;
    static int currentThread;

    static const int totalCpu;

public slots:
    void createPacket();

signals:
    void createFinished();

private:
    clsPacketOperation* packet;
    QString path;

    int returnUsageRate();
};

#endif // CLSPACKETWORKER_H
