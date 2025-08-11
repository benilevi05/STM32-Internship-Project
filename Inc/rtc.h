#ifndef RTC_H
#define RTC_H

#include "stm32f7xx_hal.h"
#include <stdint.h>

void initialize_rtc(RTC_HandleTypeDef *hrtc);
void rtc_set_time(int hour, int minute, int second);
void rtc_get_time(RTC_TimeTypeDef *gTime);
void rtc_increment_time(int hour, int minute, int second);
void rtc_reset_seconds();
#endif