#ifndef __CRC16_H_
#define __CRC16_H_

#include <stdint.h>

uint16_t crc16xmodem(const uint8_t *buffer, int size);

#endif