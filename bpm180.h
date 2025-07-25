#include <stdint.h>
#include "stm32f7xx_hal.h"

#define BMP180_ADRESS 0xEE
#define BMP180_REG_CONTROL 0xF4
#define BMP180_REG_RESULT 0xF6

#define BMP180_CALIB_CONTROL_BASE 0xAA
#define BMP180_TEMP_CONTROL 0x2E


typedef enum {
	AC1 = 0, AC2, AC3, AC4, AC5, AC6, B1, B2, MB, MC, MD
} BPM_Calibiration_Coefs_E;

void BPM_Read_Calibration_Coefficients(I2C_HandleTypeDef *hi2c1);
long BPM_Read_Raw_Temperature(I2C_HandleTypeDef *hi2c1);
int BPM_Read_True_Temperature(I2C_HandleTypeDef *hi2c1);

