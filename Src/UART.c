#include "UART.h"

char temperatureInASCII[6];
char clockInASCII[13];
char message[] = "Temperature: ";

void intToASCII(int num) {
	uint8_t a, b;
	a = num %10;
	b = num / 10;
	temperatureInASCII[0] = '0' + b;
	temperatureInASCII[1] = '0' + a;
	temperatureInASCII[2] = '\r';
	temperatureInASCII[3] = '\n';
}

void message_temperature(UART_HandleTypeDef *huart, int realTemperature) {
	uint8_t temp = realTemperature;
	intToASCII(temp);
	HAL_UART_Transmit(huart, (uint8_t*)message , strlen(message), 100);
	HAL_UART_Transmit(huart, (uint8_t*)temperatureInASCII , strlen(temperatureInASCII), 10);
}

void message_clock(UART_HandleTypeDef *huart) {
	uint8_t h1, h2, m1, m2;
        RTC_TimeTypeDef gTime;
        rtc_get_time(&gTime);
	h2 = gTime.Hours / 10;
	h1 = gTime.Hours % 10;
	m2 = gTime.Minutes / 10;
	m1 = gTime.Minutes % 10;
	clockInASCII[0] = 'T';
	clockInASCII[1] = 'i';
	clockInASCII[2] = 'm';
	clockInASCII[3] = 'e';
	clockInASCII[4] = '-';
	clockInASCII[5] = '>';
	clockInASCII[6] = '0' + h2;
	clockInASCII[7] = '0' + h1;
	clockInASCII[8] = ':';
	clockInASCII[9] = '0' + m2;
	clockInASCII[10] = '0' + m1;
	clockInASCII[11] = '\r';
	clockInASCII[12] = '\n';
	t_NMEA_Segment_Fast* pSegment = construct_segment_from_string(clockInASCII, 0, strlen(clockInASCII));
	send_single_segment_formatted(huart, pSegment);
}
