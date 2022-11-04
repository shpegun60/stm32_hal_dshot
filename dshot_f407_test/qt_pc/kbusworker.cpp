#include "kbusrawparser.h"
#include "kbusworker.h"
#include <QInputDialog>
#include <QSerialPort>
#include <QDebug>


KBusWorker::KBusWorker(QObject *parent) : QObject(parent)
{
    //m_bus = new QSerialPort(this);
    //auto txt = QInputDialog::getText(0, "com name", "", QLineEdit::Normal, "COM1");
    //m_bus->setPortName(txt);
    //m_bus->setParity(QSerialPort::NoParity);
    //m_bus->setStopBits(QSerialPort::OneStop);
    //m_bus->setFlowControl(QSerialPort::NoFlowControl);
    //m_bus->setBaudRate(QSerialPort::Baud38400); //Baud115200 Baud38400
    //m_bus->open(QIODevice::ReadWrite);

    m_rawDataParser = new KBusRawParser();
    m_rawDataParser->setupFailCallbak([this](int err){
        //qDebug() << "onPackFailed";
        onPacketFailed(err);
    });
    m_rawDataParser->setupCallbak([this](QByteArray a){
        //qDebug() << "onPacketreceived";
        onPacketReceived(a);
    });
    //connect(m_bus, &QSerialPort::readyRead, this, [&](){
    //    auto dt = this->m_bus->readAll();
    //    m_rawDataParser->addRawData(dt);
    //});
}

void KBusWorker::setupBus(KBusDevice *bus)
{
    m_bus = bus;
    m_bus->setupReadPacketCallback([this](const QByteArray &ar){
        m_rawDataParser->addRawData(ar);
    });
}

void KBusWorker::onPacketReceived(QByteArray)
{

}

void KBusWorker::onPacketFailed(int)
{

}

void KBusWorker::sendPacket(QByteArray data)
{
    auto pack = m_rawDataParser->createPackForFPGA(1, data);
    qDebug() << "DEBUG2" << pack.toHex();
    m_bus->writePacket(pack);
}


