#include "4D7S_drive.h"

const LED_t LED1 = { .Port = GPIOC, .Pin = GPIO_PIN_7 };
const LED_t LED2 = { .Port = GPIOC, .Pin = GPIO_PIN_6 };
const LED_t LED3 = { .Port = GPIOG, .Pin = GPIO_PIN_6 };
const LED_t LED4 = { .Port = GPIOB, .Pin = GPIO_PIN_4 };
const LED_t LED5 = { .Port = GPIOG, .Pin = GPIO_PIN_7 };
const LED_t LED6 = { .Port = GPIOI, .Pin = GPIO_PIN_0 };
const LED_t LED7 = { .Port = GPIOH, .Pin = GPIO_PIN_6 };
const LED_t LED8 = { .Port = GPIOI, .Pin = GPIO_PIN_3 };

const LED_t LED_Digit_4 = { .Port = GPIOF, .Pin = GPIO_PIN_6 };
const LED_t LED_Digit_3 = { .Port = GPIOF, .Pin = GPIO_PIN_7 };
const LED_t LED_Digit_2 = { .Port = GPIOF, .Pin = GPIO_PIN_8 };
const LED_t LED_Digit_1 = { .Port = GPIOF, .Pin = GPIO_PIN_9 };
const LED_t *leds_segment[] = {&LED1, &LED2, &LED3, &LED4, &LED5, &LED6, &LED7, &LED8};
const LED_t *leds_digit[] = {&LED_Digit_1, &LED_Digit_2, &LED_Digit_3, &LED_Digit_4};

uint32_t segment_map[] = {
		NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9
};

volatile int number_shown_int_digits[4]; //The number which will be displayed every 1ms.
volatile short counter_temp = 0;
volatile short counter_clock = 0;
short negativeFlag = 0;

short mode = 0;

/**'
 * Every 1 ms the timer interrupt displays a number. This number is set by calling this function.
 * Calling this function once is enough, call this to change the number again.
 * @param n: Number to be set.
 */
void display_number(int n) {
	if (n < 0) {negativeFlag = 1; n*=-1;}
		else {negativeFlag = 0;}
	for (short i = 0; i < 4; i++) {
		number_shown_int_digits[i] = n %10;
		if (i < 3) {n/=10;}
	}
}

/**
 * Sets the display mode which alters the style in which the number is displayed in.
 * @param modeToSet: 0 for Temperature Mode, 1 for Counter Mode
 */
void set_mode(short modeToSet) {
	mode = modeToSet;
}

short get_mode() {
	return mode;
}

void turn_all_leds_off() {
	for (int i = 0; i < NUM_LEDS_DIGIT; i++) {LED_OFF(*leds_digit[i]);}
	for (int i = 0; i < NUM_LEDS_SEGMENT; i++) {LED_ON(*leds_segment[i]);}
}

/**'
 * Displays a given digit.
 * LED_OFF() makes the given pin ground which in a common anode activates the LED.
 * @param digit: Digit to be displayed
 */
void display_digit(int digit) {
	uint32_t segment_mapping = segment_map[digit];
	for (int i = 0; i < 8; i++) {
		if (segment_mapping  & (1 << i)) {
			LED_OFF(*leds_segment[i]); //OFF TURNS ON COMMON ANODE
		} else {
			LED_ON(*leds_segment[i]);
		}
	}
}

/**
 * Switches to the specific digit for modification. Index is 0-based.
 */
void switch_digit(int i) {
	for(int j = 0; j < 4; j++) {
		if(j == i) {
			LED_ON(*leds_digit[j]);
		} else {
			LED_OFF(*leds_digit[j]);
		}
	}
}

/**
 * Displays the number in a temperature style. Frequency per digit is 250Hz.
 */
void display_temp() {
	if (counter_temp == 0) {
		if (negativeFlag == 1) {
			switch_digit(0);
			display_symbol('-');
		}
		counter_temp = 1;
	} else if (counter_temp == 1) {
		switch_digit(1);
		display_digit(number_shown_int_digits[1]);
		counter_temp = 2;
	} else if (counter_temp == 2) {
		switch_digit(2);
		display_digit(number_shown_int_digits[0]);
		counter_temp = 3;
	} else if (counter_temp == 3){
		switch_digit(3);
		display_symbol('C');
		counter_temp = 0;
	}
}

/**
 * 'Displays the number in a count style. Frequency per digit is 250Hz.
 */
void display_clock(t_ClockMode clockMode) {
  if (clockMode == BOTH) {
	if (counter_clock == 0) {
		switch_digit(0);
		display_digit(number_shown_int_digits[3]);
		counter_clock = 1;
	} else if (counter_clock == 1) {
		switch_digit(1);
		display_digit(number_shown_int_digits[2]);
		counter_clock = 2;
	} else if (counter_clock == 2) {
		switch_digit(2);
		display_digit(number_shown_int_digits[1]);
		counter_clock = 3;
	} else if (counter_clock == 3){
		switch_digit(3);
		display_digit(number_shown_int_digits[0]);
		counter_clock = 4;
	} else if (counter_clock == 4) { //colon
		turn_all_leds_off();
		LED_OFF(LED8); //turn on decimal
		LED_ON(LED_Digit_2);
		//LED_ON(LED_Digit_3);
		counter_clock = 0;
	}
  } else if (clockMode == HOUR_ONLY) {
      if (counter_clock % 4 == 0) {
            switch_digit(0);
            display_digit(number_shown_int_digits[3]);
            counter_clock = 1;
      } else if (counter_clock % 4 == 1) {
            switch_digit(1);
            display_digit(number_shown_int_digits[2]);
            counter_clock = 2;
      } else if (counter_clock % 4 == 2 || counter_clock % 4 == 3) {
            turn_all_leds_off();
            counter_clock = (counter_clock + 1) % 4;
      }
  } else if (clockMode == MINUTE_ONLY) {
      if (counter_clock % 2 == 0) {
            switch_digit(2);
            display_digit(number_shown_int_digits[1]);
            counter_clock = 1;
      } else if (counter_clock % 2 == 1) {
            switch_digit(3);
		display_digit(number_shown_int_digits[0]);
		counter_clock = 2;
      } else if (counter_clock % 4 == 2 || counter_clock % 4 == 3) {
            turn_all_leds_off();
            counter_clock = (counter_clock + 1) % 4;
      }
  }
}

void display_symbol(char c) {
	uint32_t segment_mapping;
	if (c == 'C') {segment_mapping = SYMBOL_C;}
	else if (c == '-') {segment_mapping = SYMBOL_NEGATIVE;}
	for (int i = 0; i < 8; i++) {
		if (segment_mapping  & (1 << i)) {
			LED_OFF(*leds_segment[i]); //OFF TURNS ON
		} else {
			LED_ON(*leds_segment[i]);
		}
	}
}
