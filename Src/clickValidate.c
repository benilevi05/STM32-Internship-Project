#include "clickValidate.h"

const int DebounceMS = 75;
volatile uint32_t lastClick = 0;

bool isDebounce() {
	uint32_t now = HAL_GetTick();

	if (now - lastClick < DebounceMS) {
		return true;
	} else {
		lastClick = now;
		return false;
	}
}
