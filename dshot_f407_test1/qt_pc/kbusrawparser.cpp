#include "kbusrawparser.h"
#include <QDebug>

KBusRawParser::KBusRawParser()
{

}

void KBusRawParser::addRawData(QByteArray ar)
{
    m_buffer += ar;
    _proceedPacket();
}

void KBusRawParser::setupCallbak(std::function<void (QByteArray)> foo)
{
    m_callback = foo;
}

void KBusRawParser::setupFailCallbak(std::function<void (int)> foo)
{
    m_failCallback = foo;
}

QByteArray KBusRawParser::createPack(QByteArray)
{


}

QByteArray KBusRawParser::createPackForFPGA(int id, QByteArray data)
{
    data.prepend(static_cast<char>(data.size()));
    //data.prepend(static_cast<char>(getLenId(0x03, data.size())));
    data.append(static_cast<char>(_calcCRC(data)));
    QByteArray arr;
    auto addByte = [&arr](char b) {
        arr.append(b);
        if (b == 0x1A) arr.append(b);
    };
    arr.append(0x1A);
    for (int i = 0, l = data.length(); i < l; i++) {
        addByte(data[i]);
    }
    return arr;
}



//void KBusRawParser::updateRaw(QByteArray data)
//{
//    data.prepend(static_cast<char>(getLenId(data.size())));
//    data.append(static_cast<char>(_calcCRC(data)));
//    //data.prepend(0x1A);

//    //QByteArray arr;
//    auto addByte = [this](char b) {
//        m_raw.append(b);
//        if (b == 0x1A) m_raw.append(b);
//    };

//    m_raw.clear();
//    m_raw.append(0x1A);
//    for (int i = 0, l = data.length(); i < l; i++) {
//        addByte(data[i]);
//    }
//}

quint8 KBusRawParser::getLenId(int id, int len)
{
    quint8 l;
    switch (len) {
    case 1: {l = 1 << 5; break;}
    case 2: {l = 2 << 5; break;}
    case 4: {l = 3 << 5; break;}
    case 8: {l = 4 << 5; break;}
    case 16: {l = 5 << 5; break;}
    //case 6: {l = 6 << 5; break;}
    //case 7: {l = 7 << 5; break;}
    default: qFatal("Incorrect Len");
    }
    return l | id;
}

bool KBusRawParser::_proceedPacket()
{
    int packetStartPos = 0;
    int len = 0;
    bool inPack = 0;
    int lastPos = -1;

    bool lastWasSB = (m_buffer[0] == m_SB);

    auto addDataByte = [&](char b){
        m_packetData.append(b);
    };

    auto proceedCrc = [&](int i) {//check crc
            quint8 crc_calulated = _calcCRC(m_buffer[packetStartPos+1], m_packetData);
            quint8 crc_received = static_cast<quint8>(m_buffer[i]);
            bool crc_ok = crc_calulated == crc_received;
            if (crc_ok) {
                if (m_callback != nullptr)
                    m_callback(m_packetData);
            }else {
                if (m_failCallback != nullptr)
                    qDebug() << "CRC";
                    m_failCallback(INCORRECT_CRC);
            }
            inPack = 0;
            lastPos = i;
    };
    //enum {WAIT_PACK, LEN, DATA, CRC} state = WAIT_PACK;
    //
    //for (int i = 1, sz = m_buffer.size(); i < sz; ++i) {
    //    //look if pack now m_buffer[i] == SB && m_buffer[i-1] == SB
    //    if (m_buffer[i] == m_SB && )
    //}


    for (int i = 1, sz = m_buffer.size(); i < sz; ++i) {
        if (lastWasSB) {
            if (m_buffer[i] == m_SB) {
                if (m_packetData.length() == len) {
                    proceedCrc(i);
                } else {
                    addDataByte(m_SB);
                }
                lastWasSB = 0;
            } else { // new packed {SB}{!SB}
                if (inPack == 1) {
                    m_failCallback(NEWPACK_UNTIL_RECEIVE_OLD);
                    lastPos = i-2;//???????????
                }
                inPack = 1;
                packetStartPos = i-1;
                //len = m_buffer[i]; // modified by shpegun60 old version of protocol!!!
                len = (m_buffer[i] > m_SB) ? (m_buffer[i] - 1) : m_buffer[i];
                m_packetData.clear();
                lastWasSB = 0;
            }
        } else { // !lastWasSB
            if (inPack == 1) {
                if (m_buffer[i] != m_SB) {
                    if (m_packetData.length() == len) {
                        proceedCrc(i);
                    } else if (m_buffer[i] != m_SB) { // len of packetData < len
                        //if (m_buffer[i] != m_SB) {
                            addDataByte(m_buffer[i]);
                        //}
                    }
                } //if (m_buffer[i] != m_SB)
            }
            lastWasSB = (m_buffer[i] == m_SB);
        }

    }
/*
    for (int i = 1, sz = m_buffer.size(); i < sz; ++i) {
        if ((m_buffer[i-1] == m_SB) && (m_buffer[i] != m_SB)) {

            packetStartPos = i-1;
            len = m_buffer[i];
            m_packetData.clear();
            if (inPack == 1) {
                m_failCallback(NEWPACK_UNTIL_RECEIVE_OLD);
                lastPos = i-2;
            }
            inPack = 1;
        } else if (inPack == 1) {
            if (m_packetData.length() == len) {
                //check crc
                bool crc_ok = 1;
                if (crc_ok) {
                    m_callback(m_packetData);
                }else {
                    m_failCallback(INCORRECT_CRC);
                }
                inPack = 0;
                lastPos = i;
            }
            if (!((m_buffer[i-1] == m_SB) && (m_buffer[i] == m_SB))) {
                m_packetData.append(m_buffer[i]);
            }
        }
    }
*/

    if (lastPos > 0) {
        m_buffer.remove(0, lastPos);
    }
    return 0;
}


quint8 KBusRawParser::_calcCRC(QByteArray ar)
{
    quint8 crc = 0xFF;
    for(int l = 0; l<ar.length(); l++ )
    {
        crc ^= ar[l];
        for (int i = 0; i < 8; i++)
           crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
    return crc;
}

quint8 KBusRawParser::_calcCRC(char len, QByteArray ar)
{
    quint8 crc = 0xFF;
    ar.prepend(len);
    for(int l = 0; l<ar.length(); l++ )
    {
        crc ^= ar[l];
        for (int i = 0; i < 8; i++)
           crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
    return crc;
}
