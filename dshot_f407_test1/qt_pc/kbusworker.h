#ifndef KBUSWORKER_H
#define KBUSWORKER_H

#include <QObject>
#include <functional>

class KBusDevice
{
public:
    //virtual ~KBusDevice() = 0;
    virtual void writePacket(const QByteArray &) = 0;
    virtual void setupReadPacketCallback(std::function<void(const QByteArray &)>) = 0;
};

class KBusRawParser;
class QSerialPort;
class KBusWorker : public QObject
{
    Q_OBJECT
public:
    explicit KBusWorker(QObject *parent = nullptr);
    void setupBus(KBusDevice *);

protected:
    virtual void onPacketReceived(QByteArray);
    virtual void onPacketFailed(int);

public:
    void sendPacket(QByteArray);

private:
    //QSerialPort *m_bus;
    KBusRawParser *m_rawDataParser;
    KBusDevice *m_bus;
};

#endif // KBUSWORKER_H
