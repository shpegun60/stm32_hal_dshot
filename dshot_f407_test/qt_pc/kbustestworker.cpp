#include "kbustestworker.h"

#include <QDataStream>

KBusTestWorker::KBusTestWorker(QObject *parent) : KBusWorker(parent)
{

}

void KBusTestWorker::send(QString cmd)
{
    cmd = cmd.remove(' ').remove('_');
    auto r = QByteArray::fromHex(cmd.toUtf8());
    sendPacket(r);
}

void KBusTestWorker::onPacketReceived(QByteArray ar)
{
    emit packetReceived(ar);
}

void KBusTestWorker::onPacketFailed(int)
{
}
