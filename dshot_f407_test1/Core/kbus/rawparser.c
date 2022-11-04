#include "rawparser.h"

uint8_t RawParser_Init(RawParser_t *desc, RawParser_Init_t *desc_init)
{
	desc->writePos = 0;
	desc->m_frameLen = 0;
	desc->m_frameCrc = 0xFF;

	desc->packReady = 0;
	desc->m_lastWasSB = 0;
	desc->m_receiveNow = 0;
	desc->m_pos = 0;
	desc->readPos = 0;

    desc->startByte = desc_init->startByte;

    desc->maxFrameSize = desc_init->maxFrameSize;
    desc->outputFrameBuff = (uint8_t *)malloc(desc_init->maxFrameSize);
    if (desc->outputFrameBuff == NULL) {
        return  1;
    }

    desc->inputFrameBuff = (uint8_t *)malloc(desc_init->maxFrameSize);
    if (desc->inputFrameBuff == NULL) {
        return  2;
    }

    desc->inputBytesBuffSize = desc_init->inputBytesBuffSize;
    desc->inputBytesBuff = (uint8_t *)malloc(desc_init->inputBytesBuffSize);
    if (desc->inputBytesBuff == NULL) {
        return  3;
    }

    return 0;
}

void RawParser_DeInit(RawParser_t *desc)
{
   free(desc->outputFrameBuff);
   free(desc->inputFrameBuff);
   free(desc->inputBytesBuff);

   desc->outputFrameBuff = NULL;
   desc->inputFrameBuff= NULL;
   desc->inputBytesBuff = NULL;
}

void RawParser_ProceedByte(RawParser_t *desc, uint8_t byte)
{
	if (desc->inputBytesBuff == NULL) {
		return;
	}

    desc->inputBytesBuff[desc->writePos] = byte;
    desc->writePos = (desc->writePos + 1) % desc->inputBytesBuffSize;
}

RawParser_Frame *RawParser_ShieldFrame(RawParser_t *desc, uint8_t *data, uint8_t dataSize)
{
    uint8_t crc = 0xFF;
    uint8_t writePos = 0;

    desc->outputFrameBuff[writePos++] = desc->startByte;

    if (dataSize >= desc->startByte) {
        crc = _RawParser_proceedCrc(crc, (dataSize + 1));
        desc->outputFrameBuff[writePos++] = (dataSize + 1);

        if ((dataSize + 1) == desc->startByte)
            desc->outputFrameBuff[writePos++] = (dataSize + 1);

    } else {
        crc = _RawParser_proceedCrc(crc, dataSize);
        desc->outputFrameBuff[writePos++] = dataSize;

        if (dataSize == desc->startByte)
            desc->outputFrameBuff[writePos++] = dataSize;
    }


    for (uint8_t i = 0; i < dataSize; i++) {
        crc = _RawParser_proceedCrc(crc, data[i]);
        desc->outputFrameBuff[writePos++] = data[i];

        if (data[i] == desc->startByte)
            desc->outputFrameBuff[writePos++] = data[i];
    }


    desc->outputFrameBuff[writePos++] = crc;
    if (crc == desc->startByte)
        desc->outputFrameBuff[writePos++] = crc;

    desc->frameTX.data = desc->outputFrameBuff;
    desc->frameTX.size = writePos;
    return &desc->frameTX;
}

uint8_t RawParser_GetMaxFrameSize(RawParser_t *desc)
{
	return desc->maxFrameSize;
}

uint8_t _RawParser_proceedCrc(uint8_t crc, uint8_t ch)
{
    crc ^= ch;
    for (int i = 0; i < 8; i++) {
        crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
    return crc;
}

void _RawParser_proceedUnshieldedDataStream(RawParser_t *desc, uint8_t ch, uint8_t newFrame)
{
    if (newFrame) {
        desc->m_frameCrc = 0xFF;
        desc->m_receiveNow = 1;
        desc->m_pos = 0;
    }

    if (!desc->m_receiveNow)
        return;

    if (desc->m_pos == 0) {
        desc->m_frameLen = ch;

        if (desc->m_frameLen > desc->startByte)
            desc->m_frameLen -= 1;

    } else if ((desc->m_pos - 1) < desc->m_frameLen) {
        desc->inputFrameBuff[desc->m_pos-1] = ch;

    } else if ((desc->m_pos - 1) == desc->m_frameLen)  {
        if(desc->m_frameCrc == ch)
            desc->packReady = 1;

    } else {
        return;
    }

    desc->m_pos++;
    desc->m_frameCrc = _RawParser_proceedCrc(desc->m_frameCrc, ch);
}

RawParser_Frame *RawParser_ReadUnshieldedFrame(RawParser_t *desc)
{
    while (desc->readPos != desc->writePos) {
        uint8_t ch = desc->inputBytesBuff[desc->readPos];
        desc->readPos = (desc->readPos + 1) % desc->inputBytesBuffSize;

        if (desc->m_lastWasSB) {
            if (ch == desc->startByte) {
                _RawParser_proceedUnshieldedDataStream(desc, ch, 0);
            } else {
                _RawParser_proceedUnshieldedDataStream(desc, ch, 1);
            }

            desc->m_lastWasSB = 0;

        } else if (ch == desc->startByte) {
            desc->m_lastWasSB = 1;
        } else {
            _RawParser_proceedUnshieldedDataStream(desc, ch, 0);
        }

        if (desc->packReady) {
            desc->packReady = 0;
            desc->frameRX.data = desc->inputFrameBuff;
            desc->frameRX.size = desc->m_frameLen;
            return &desc->frameRX;
        }
    }

    desc->frameRX.data = desc->inputFrameBuff;
    desc->frameRX.size = 0;
    return &desc->frameRX;
}
