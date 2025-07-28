#include <stdbool.h>
#include <stdint.h>

typedef enum {
	SINGLE_CLICK = 0,
	DOUBLE_CLICK,
	LONG_PRESS
} t_Click;

bool isDebounce();
