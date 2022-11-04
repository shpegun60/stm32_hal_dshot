#ifndef KBUSTESTWORKER_H
#define KBUSTESTWORKER_H

#include "kbusworker.h"

#include <QObject>

class KBusTestWorker : public KBusWorker
{
    Q_OBJECT
public:
    explicit KBusTestWorker(QObject *parent = nullptr);

signals:
    void packetReceived(const QByteArray);

public slots:
    void send(QString);
    // KBusWorker interface
protected:
    virtual void onPacketReceived(QByteArray) override;
    virtual void onPacketFailed(int) override;
};

#endif // KBUSTESTWORKER_H
