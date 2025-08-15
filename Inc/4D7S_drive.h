#ifndef FDSS_H
#define FDSS_H

#include <stdint.h>
#include "stm32f7xx_hal.h"

typedef struct {
    GPIO_TypeDef *Port;  // e.g. GPIOA, GPIOB
    uint16_t     Pin;    // e.g. GPIO_PIN_7
} LED_t;

typedef enum {
	TEMP_MODE = 0,
	CLOCK_MODE = 1,
	SETTINGS_MODE = 2
} MODE_t;

#define NUM_LEDS_SEGMENT  (sizeof(leds_segment)/sizeof(leds_segment[0]))
#define NUM_LEDS_DIGIT  (sizeof(leds_digit)/sizeof(leds_digit[0]))

#define LED_ON(led)    HAL_GPIO_WritePin((led).Port, (led).Pin, GPIO_PIN_SET)
#define LED_OFF(led)   HAL_GPIO_WritePin((led).Port, (led).Pin, GPIO_PIN_RESET)
#define LED_TOGGLE(led) HAL_GPIO_TogglePin((led).Port, (led).Pin)

#define NUM0 (uint32_t)0b00111111
#define NUM1 (uint32_t)0b00000110
#define NUM2 (uint32_t)0b01011011
#define NUM3 (uint32_t)0b01001111
#define NUM4 (uint32_t)0b01100110
#define NUM5 (uint32_t)0b01101101
#define NUM6 (uint32_t)0b01111101
#define NUM7 (uint32_t)0b00000111
#define NUM8 (uint32_t)0b01111111
#define NUM9 (uint32_t)0b01101111

#define SYMBOL_C (uint32_t)0b00111001
#define SYMBOL_NEGATIVE (uint32_t)0b01000000


typedef enum {
  BOTH = 0,
  HOUR_ONLY,
  MINUTE_ONLY
} t_ClockMode;

//Public Functions
void display_number(int n);
void set_mode(short modeToSet);
short get_mode();

//Private Functions
void turn_all_leds_off();
void display_digit(int x);
void switch_digit(int i);
void display_temp();
void display_clock(t_ClockMode clockMode);
void display_symbol(char c);

#endif
