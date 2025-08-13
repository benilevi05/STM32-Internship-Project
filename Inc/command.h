#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "rtc.h"
#include "UART.h"
#include "bpm180.h"
#include "eeprom.h"

#define COMMAND_COUNT 5

extern int minThreshold;
extern int maxThreshold;

void commanndSetChannels(UART_HandleTypeDef *huart, I2C_HandleTypeDef *hi2c1);
bool commandIdentify(char command[]);
void setMinThreshold(int valToSet);
void setMaxThreshold(int valToSet);

#endif