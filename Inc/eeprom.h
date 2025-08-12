#ifndef EEPROM_H
#define EEPROM_H

#include "aes.h"
#include "stm32f7xx_hal.h"

#define EEPROM_ADRESS (uint8_t) 0x50
#define EEPROM_MEM_ADRESS 0x00
#define EEPROM_PAGE_SIZE 8 //bytes
#define EEPROM_BYTE_SIZE 256

void eeprom_store_temperature(int temperature, int maxThreshold, int minThreshold);
void eeprom_get_temperature(int output[3]);
#endif