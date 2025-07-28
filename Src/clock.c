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
