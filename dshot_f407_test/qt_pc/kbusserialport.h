#ifndef KBUSSERIALPORT_H
#define KBUSSERIALPORT_H

#include "kbusworker.h"

#include <QObject>
#include <QSerialPort>


class KBusSerialPort : public QObject, public KBusDevice
{
    Q_OBJECT
public:
    explicit KBusSerialPort(QObject *parent = nullptr);
    QSerialPort *port = nullptr;

public:
    virtual void writePacket(const QByteArray &) override;
    virtual void setupReadPacketCallback(std::function<void (const QByteArray &)>) override;
};

#endif // KBUSSERIALPORT_H
