#include "rtc.h"

RTC_HandleTypeDef rtc;

void initialize_rtc(RTC_HandleTypeDef *hrtc) {
   rtc = *hrtc;
}

void rtc_set_time(int hour, int minute, int second) {
  RTC_TimeTypeDef sTime = {0};
  sTime.Seconds = second;
  sTime.Minutes = minute;
  sTime.Hours = hour;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  
  HAL_RTC_SetTime(&rtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTCEx_BKUPWrite(&rtc, RTC_BKP_DR1, 0x2345);
}

void rtc_get_time(RTC_TimeTypeDef* gTime) {
  RTC_DateTypeDef gDate;
  HAL_RTC_GetTime(&rtc, gTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&rtc, &gDate, RTC_FORMAT_BIN);

}

void rtc_increment_time(int hour, int minute, int second) {
  RTC_TimeTypeDef time;
  rtc_get_time(&time);
  time.Hours += hour;
  time.Minutes += minute;
  time.Seconds += second;
  rtc_set_time(time.Hours, time.Minutes, time.Hours);
}

void rtc_reset_seconds() {
  RTC_TimeTypeDef time;
  rtc_get_time(&time);
  time.Seconds = 0;
  rtc_set_time(time.Hours, time.Minutes, time.Hours);
}

/** Write these to your program before calling any of the other functions above.
  initialize_rtc(&hrtc);
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x2345) {
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x2345);
    rtc_set_time(12, 0, 3);
  }
*/