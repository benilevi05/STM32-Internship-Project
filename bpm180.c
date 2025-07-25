#include "bpm180.h"

volatile short BPM_Calibiration_Coefs[11];

/**
 * This function returns the actual temperature from the sensor in mC's.
 * It uses the raw temperature data from the sensor and using some of the coefficients calculates the real temperature.
 * Divide by 10 to get the actual temperature. 240mC -> 24C.
 * @param *hi2c1: The I2C channel which is communicating.
 */
int BPM_Read_True_Temperature(I2C_HandleTypeDef *hi2c1) {
	BPM_Read_Calibration_Coefficients(hi2c1);
	long UT = BPM_Read_Raw_Temperature(hi2c1);
	long X1 = ((UT - BPM_Calibiration_Coefs[AC6]) * BPM_Calibiration_Coefs[AC5]) / pow(2, 15);
	long X2 = (BPM_Calibiration_Coefs[MC] * pow(2, 11)) / (X1 + BPM_Calibiration_Coefs[MD]);
	long B5 = X1+ X2;
	int T = (B5 + 8) / pow(2, 4);
	return T;
}

/**
 * This function returns the raw temperature data from the sensor.
 * @param *hi2c1: The I2C channel which is communicating.
 */
long BPM_Read_Raw_Temperature(I2C_HandleTypeDef *hi2c1) {
	volatile short testError = 0;
	volatile short test = 0;
	HAL_StatusTypeDef ret;
	uint8_t buf[2];

	uint8_t cmd = BMP180_TEMP_CONTROL;

	ret = HAL_I2C_Mem_Write(hi2c1, BMP180_ADRESS, BMP180_REG_CONTROL, I2C_MEMADD_SIZE_8BIT, &cmd, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) {
		testError++;
	}
	test++;
	ret = HAL_I2C_Mem_Read(hi2c1, BMP180_ADRESS, BMP180_REG_RESULT, I2C_MEMADD_SIZE_8BIT, buf, 2, HAL_MAX_DELAY);
	if (ret != HAL_OK) {
		testError++;
	}
	test++;
	long result = (buf[0] << 8) | buf[1];
	return result;
}

/**
 * This function gets all the calibration coefficients and stores them in the array BPM_Calibiration_Coefs[].
 * @param *hi2c1: The I2C channel which is communicating.
 */
void BPM_Read_Calibration_Coefficients(I2C_HandleTypeDef *hi2c1) {

	for (int i = 0; i < 11; i++) {
		HAL_StatusTypeDef ret;
		uint8_t buf[2];
		uint8_t cmd = BMP180_CALIB_CONTROL_BASE + (i * 2);

		ret = HAL_I2C_Mem_Read(hi2c1, BMP180_ADRESS, cmd, I2C_MEMADD_SIZE_8BIT, buf, 2, HAL_MAX_DELAY);
		BPM_Calibiration_Coefs[i] = (buf[0] << 8) | buf[1];
	}
}

