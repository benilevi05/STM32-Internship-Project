//My Timer is slightly too fast. About 1 minute per 40 minutes.
//Similar problem below:
//https://community.st.com/t5/stm32-mcus-products/stm32h7-timer-interrupt-slightly-too-fast/td-p/145539

#include "clock.h"

extern t_Clock clock = {0, 0, 0};


void checkMinuteHour() {
	if (clock.second >= 60) {clock.minute += 1; clock.second -= 60;} //in an anomaly we don't loose the time with >= instead of ==
	if (clock.minute >=60) {clock.hour += 1; clock.minute -= 60;}
	if (clock.hour >=24) {clock.hour = 0;}
}

void secondPassed() {
	clock.second += 1;
	checkMinuteHour();
}

short getHour() {
	return clock.hour;
}

short getMinute() {
	return clock.minute;
}

void incrementMinute(int val) {
	clock.minute += val;
	if (clock.minute >=60) {clock.minute -= 60;}
}

void incrementHour(int val) {
	clock.hour += val;
	if (clock.hour >= 24) {clock.hour -= 24;}
}

void resetSeconds() {
	clock.second = 0;
}

void setClock(int val) {
	clock.hour = val / 100;
	clock.minute = val %100;
	clock.second = 0;
}
