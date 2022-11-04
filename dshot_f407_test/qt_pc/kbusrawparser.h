#ifndef KBUSRAWPARSER_H
#define KBUSRAWPARSER_H

#include <QByteArray>
#include <functional>


class KBusRawParser
{
public:
    enum Fails{NEWPACK_UNTIL_RECEIVE_OLD = 1, INCORRECT_CRC};

    KBusRawParser();

    void addRawData(QByteArray ar);
    void setupCallbak(std::function<void(QByteArray)>);
    void setupFailCallbak(std::function<void(int)>);

    QByteArray createPack(QByteArray);
    QByteArray createPackForFPGA(int id, QByteArray);
private:
    const char m_SB = 0x1A;
    bool _proceedPacket();

    QByteArray m_buffer;
    std::function<void(QByteArray)> m_callback = nullptr;
    std::function<void(int)> m_failCallback = nullptr;
    QByteArray m_packetData;


    quint8 _calcCRC(QByteArray ar);
    quint8 _calcCRC(char len, QByteArray ar);

    quint8 getLenId(int id, int len);
};

#endif // KBUSRAWPARSER_H
