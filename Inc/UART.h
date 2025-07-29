#ifndef UART_H
#define UART_H

#include "stm32f7xx_hal.h"
#include <string.h>
#include "NMEA.h"
#include "clock.h"

void message_temperature(UART_HandleTypeDef *huart, int realTemperature);
void message_clock(UART_HandleTypeDef *huart);
#endif
