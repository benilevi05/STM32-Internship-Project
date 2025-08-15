/**
 * Another Timer to increment every second for clock
 * Timer will be reset when the clock is set from the button.
 * A variable count will keep track of the time.
 * A function will return minute and hour given count.
 * A function will return count when given minute and hour.
 * OR
 * Three variables hour, minute, and second.
 * A function call will check if second is == 60 and increment minute if it is.
 * It will also check if minute == 60 and increment hour if it is
 * ADDITIONALLY
 * We can add a fourth variable to switch between clock and temperature. Increments every second, switch when 3.
 * or we can do that in the main file.
 */
#ifndef CLOCK_H
#define CLOCK_H


typedef struct {
	short hour;
	short minute;
	short second;
} t_Clock;

void secondPassed();
short getHour();
short getMinute();
void incrementMinute(int val);
void incrementHour(int val);
void resetSeconds();
void setClock(int val);

#endif
