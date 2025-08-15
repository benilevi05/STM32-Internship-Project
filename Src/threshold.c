#include "threshold.h"

int testThresholdMax = 0;
int testThresholdMin = 0;

void checkThresholds(int temperature) {
  if (temperature < minThreshold) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
    testThresholdMin = 1;
  } else {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
    testThresholdMin = 0;
  }
  if (temperature > maxThreshold) {
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, 1);
    testThresholdMax = 1;
  } else {
    testThresholdMax = 0;
    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_0, 0);
  }
}