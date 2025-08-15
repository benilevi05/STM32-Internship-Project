#include <stdbool.h>
#include <stdint.h>
#include "stm32f7xx_hal.h"

typedef enum {
	SINGLE_CLICK = 0,
	DOUBLE_CLICK,
	LONG_PRESS
} t_Click;

bool isDebounce();
