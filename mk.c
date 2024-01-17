#include <string.h>

#include "mk.h"

#define DEVICE_ADDRESS_LENGTH 5
uint8_t DEVICE_ADDRESS[DEVICE_ADDRESS_LENGTH] = {193, 194, 195, 196, 197};

uint8_t whiteningA[] = {141, 210, 87, 161, 61, 167, 102, 176, 117, 49, 17, 72, 150, 119, 248, 227, 70, 233, 171, 208, 158, 83, 51, 216, 186, 152, 8, 36, 203, 59, 252, 113, 163, 244, 85};
uint8_t whiteningB[] = {199, 141, 210, 87, 161, 61, 167, 102, 176, 117, 49, 17, 72, 150, 119, 248, 227};

const size_t encodedCommandDataOffset = 15;
const size_t payloadDataSize = 10;
uint8_t adv_data_template[] = {
  0x02, 0x01, 0x02, 0x15, 0xff, 0x00, 0xc2, 0x6d, 0xb6, 0x43, 0xcf, 0x7e, 0x8f, 0x47, 0x11, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t reverseByte(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void whitening_encode(uint8_t *data, const uint8_t *whitening, int len) {
    for (int i = 0; i < len; i++) data[i] ^= whitening[i];
}

uint32_t crc16(uint8_t *addressData, int32_t addressDataLength, uint8_t *commandData, int32_t commandDataLength){
  uint8_t cmdByte;
  uint16_t cmdShort;
  int32_t addressIndex;
  uint32_t bitIndex;
  uint16_t crcTempA;
  uint16_t crcTempB;
  uint32_t result;
  uint16_t crc;
  
  crc = 0xffff;
  // Calculate CRC16 for addressData
  if (0 < addressDataLength) {
    addressIndex = addressDataLength + -1;
    crc = 0xffff;
    do {
      crcTempB = ((uint16_t)(uint8_t)addressData[addressIndex] << 8 ^ crc) * 2;
      crcTempA = crcTempB ^ 0x1021;
      if (-1 < (int16_t)((uint16_t)(uint8_t)addressData[addressIndex] << 8 ^ crc)) {
        crcTempA = crcTempB;
      }
      crc = crcTempA * 2 ^ 0x1021;
      if (-1 < (int16_t)crcTempA) {
        crc = crcTempA * 2;
      }
      crcTempB = crc * 2 ^ 0x1021;
      if (-1 < (int16_t)crc) {
        crcTempB = crc * 2;
      }
      crc = crcTempB * 2 ^ 0x1021;
      if (-1 < (int16_t)crcTempB) {
        crc = crcTempB * 2;
      }
      crcTempB = crc * 2 ^ 0x1021;
      if (-1 < (int16_t)crc) {
        crcTempB = crc * 2;
      }
      crc = crcTempB * 2 ^ 0x1021;
      if (-1 < (int16_t)crcTempB) {
        crc = crcTempB * 2;
      }
      crcTempB = crc * 2 ^ 0x1021;
      if (-1 < (int16_t)crc) {
        crcTempB = crc * 2;
      }
      crc = crcTempB * 2 ^ 0x1021;
      if (-1 < (int16_t)crcTempB) {
        crc = crcTempB * 2;
      }
      addressIndex = addressIndex + -1;
    } while (addressIndex != -1);
  }
  // Calculate CRC16 for commandData
  if (0 < commandDataLength) {
    do {
      cmdByte = *commandData;
      cmdShort = (uint16_t)(uint8_t)(cmdByte >> 3 & 4 | cmdByte >> 1 & 8 | cmdByte >> 5 & 2 | cmdByte * '\x02' & 0x10 | (cmdByte & 4) << 3 | cmdByte >> 7 | cmdByte << 7 | (cmdByte & 2) << 5) << 8;
      crcTempB = (cmdShort ^ crc) * 2;
      crcTempA = crcTempB ^ 0x1021;
      if (-1 < (int16_t)(cmdShort ^ crc)) {
        crcTempA = crcTempB;
      }
      crc = crcTempA * 2 ^ 0x1021;
      if (-1 < (int16_t)crcTempA) {
        crc = crcTempA * 2;
      }
      crcTempB = crc * 2 ^ 0x1021;
      if (-1 < (int16_t)crc) {
        crcTempB = crc * 2;
      }
      crc = crcTempB * 2 ^ 0x1021;
      if (-1 < (int16_t)crcTempB) {
        crc = crcTempB * 2;
      }
      crcTempB = crc * 2 ^ 0x1021;
      if (-1 < (int16_t)crc) {
        crcTempB = crc * 2;
      }
      crc = crcTempB * 2 ^ 0x1021;
      if (-1 < (int16_t)crcTempB) {
        crc = crcTempB * 2;
      }
      crcTempB = crc * 2 ^ 0x1021;
      if (-1 < (int16_t)crc) {
        crcTempB = crc * 2;
      }
      crc = crcTempB * 2 ^ 0x1021;
      if (-1 < (int16_t)crcTempB) {
        crc = crcTempB * 2;
      }
      commandData = (uint8_t *)((uint8_t *)commandData + 1);
      commandDataLength = commandDataLength + -1;
    } while (commandDataLength != 0);
  }
  bitIndex = 0;
  cmdByte = 0xe;
  result = 0;

  // Extract the bits from the calculated CRC16 value
  do {
    if ((crc >> (bitIndex & 0x1f) & 1) != 0) {
      result = result | 1 << (cmdByte + 1 & 0x1f);
    }
    if ((crc >> (bitIndex + 1 & 0x1f) & 1) != 0) {
      result = result | 1 << (cmdByte & 0x1f);
    }
    cmdByte = cmdByte - 2;
    bitIndex = bitIndex + 2;
  } while (bitIndex != 0x10);
  // Return the calculated CRC16 checksum
  return result & 0xffff ^ 0xffff;
}

void getPayload(uint8_t *addressData, int addressDataLength, uint8_t *commandData, int commandDataLength, uint8_t* payload, int payloadSize) {
    int payloadLength = addressDataLength + commandDataLength + 18;
    int allocatedPayloadLength = payloadLength + 2;
    uint8_t data[allocatedPayloadLength];

    for (int i = 0; i < addressDataLength; i++) data[i + 18] = addressData[addressDataLength - (i + 1)];
    for (int i = 0; i < addressDataLength + 3; i++)data[i + 15] = reverseByte(data[i + 15]);
    for (int i = 0; i < commandDataLength; i++) data[i + addressDataLength + 0x12] = commandData[i];

    uint16_t crc = crc16(addressData, addressDataLength, commandData, commandDataLength);
    data[payloadLength] = crc;
    data[payloadLength + 1] = (crc >> 8);

    whitening_encode(data + 18, whiteningB, addressDataLength + commandDataLength + 2);
    whitening_encode(data, whiteningA, allocatedPayloadLength);

    memcpy(payload, data + 15 + 8, payloadSize);
}

uint8_t* getMkAdvertisementData(uint8_t* commandData, size_t commandDataSize){
  uint8_t payloadData[payloadDataSize];
  getPayload(DEVICE_ADDRESS, DEVICE_ADDRESS_LENGTH, commandData, commandDataSize, payloadData, payloadDataSize);
  memcpy(adv_data_template + encodedCommandDataOffset, payloadData, payloadDataSize);
  return adv_data_template;
}

size_t getMkAdvertisementDataSize(){
    return sizeof(adv_data_template);
}
