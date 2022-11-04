#include "kbusserialport.h"

KBusSerialPort::KBusSerialPort(QObject *parent) : QObject(parent)
{

}


void KBusSerialPort::writePacket(const QByteArray & ar)
{
    port->write(ar);
}

void KBusSerialPort::setupReadPacketCallback(std::function<void (const QByteArray &)> foo)
{
    connect(port, &QSerialPort::readyRead, port, [this,foo](){foo(port->readAll());});
}
