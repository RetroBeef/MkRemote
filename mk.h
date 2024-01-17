#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint8_t* getMkAdvertisementData(uint8_t* commandData, size_t commandDataSize);
size_t getMkAdvertisementDataSize();

#ifdef __cplusplus
}
#endif
