#ifndef RawParser_H
#define RawParser_H

#include "stdlib.h"
#include "stdint.h"

#include "main.h"

/**
  * @brief RawParser Init structure definition
  */
typedef struct {

    uint8_t startByte; // Specifies the value of start-byte.

    uint8_t maxFrameSize; // Specifies maximum amount of bytes in TX/RX frames.

    uint8_t inputBytesBuffSize; // Specifies maximum amount of bytes in receive buffer, which is filled by proceedByte function.

} RawParser_Init_t;


/**
  * @brief RawParser Frame structure definition
  */
typedef struct {

    uint8_t *data;

    uint8_t size;

} RawParser_Frame;


/**
  * @brief RawParser_DescriptorTypeDef structure definition
  */
typedef struct {

    uint8_t startByte;

    uint8_t maxFrameSize;

    RawParser_Frame frameTX;
    uint8_t *outputFrameBuff;

    RawParser_Frame frameRX;
    uint8_t inputBytesBuffSize;
    uint8_t *inputFrameBuff;
    uint8_t *inputBytesBuff;

    uint8_t writePos;
    uint8_t m_frameLen, m_frameCrc;
    uint8_t packReady, m_lastWasSB, m_receiveNow;
    uint8_t m_pos, readPos;

} RawParser_t;


/**
 * @brief Function for initialization parser. Performs memory allocation.
 * @param MRP_D - descriptor-structure of parser (as object).
 * @param MRP_Init - structure of initialization options for parser.
 * @return error-code. See the meaning of error codes in the function itself.
 */
uint8_t RawParser_Init(RawParser_t *desc, RawParser_Init_t *init_desc);


/**
 * @brief Performs free of memory
 * @param MRP_D - descriptor-structure of parser (as object).
 */
void RawParser_DeInit(RawParser_t *desc);


/**
 * @brief Push new byte in internal buffer
 * @param MRP_D - descriptor-structure of parser (as object).
 * @param byte - new incoming byte.
 */
void RawParser_ProceedByte(RawParser_t *desc, uint8_t byte);


/**
 * @brief Get data and shielded it.
 * @param MRP_D - descriptor-structure of parser (as object).
 * @param data - pointer to the data that need to be shielded.
 * @param dataSize - length of data.
 * @return Pointer to frame struct which consist of the shielded data and it size.
 */
RawParser_Frame *RawParser_ShieldFrame(RawParser_t *desc, uint8_t *data, uint8_t dataSize);


/**
 * @brief RawParser_ReadUnshieldedFrame
 * @param MRP_D - descriptor-structure of parser (as object).
 * @return Pointer to frame struct which consist of the readed data and it size.
 */
RawParser_Frame *RawParser_ReadUnshieldedFrame(RawParser_t *desc);

/**
 * @
 */
uint8_t RawParser_GetMaxFrameSize(RawParser_t *desc);

/**
 * @brief Internal function
 */
void _RawParser_proceedUnshieldedDataStream(RawParser_t *desc, uint8_t ch, uint8_t newFrame);


/**
 * @brief Internal function
 */
uint8_t _RawParser_proceedCrc(uint8_t crc, uint8_t ch);


#endif // RawParser_H
