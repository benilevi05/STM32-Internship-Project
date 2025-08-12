//Temprature Thresholds and Temperature will be stored in an external eeprom with encryption. IV for the decryption will also be stored just after the temperature data.
#include "eeprom.h"
#include "main.h"

uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

uint8_t store[16];
uint8_t output[16];

HAL_StatusTypeDef st;
int I2C_Error_Write = 0;
int I2C_Error_Read = 0;
int NotMatchingError = 0;


static HAL_StatusTypeDef eeprom_wait_ready(I2C_HandleTypeDef *hi2c, uint8_t addr7, uint32_t timeout_ms);

void eeprom_store_temperature(int temperature, int maxThreshold, int minThreshold) {
  uint8_t in[16] = { 0 }; //32 bits
  uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
  //Parse ederken in[a] * (in[a-1] - 1) sayiyi verir. Her sayi n icin a = n*2-1
  in[0] = temperature > 0 ? 2 : 0; 
  in[1] = temperature > 0 ? temperature : (-1 * temperature);
  in[2] = maxThreshold > 0 ? 2 : 0;
  in[3] = maxThreshold > 0 ? maxThreshold : (-1 * maxThreshold);
  in[4] = minThreshold > 0 ? 2 : 0;
  in[5] = minThreshold > 0 ? minThreshold : (-1 * minThreshold);
  in[6] = (uint8_t) 0xAB;
  struct AES_ctx ctx;
  AES_init_ctx_iv(&ctx, key, iv);
  AES_CBC_encrypt_buffer(&ctx, in, 16);
  st = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADRESS << 1 , EEPROM_MEM_ADRESS, I2C_MEMADD_SIZE_8BIT , (uint8_t*)&in[0], 8 , HAL_MAX_DELAY);
  if (st != HAL_OK) {I2C_Error_Write++;}
  eeprom_wait_ready(&hi2c1, EEPROM_ADRESS, 20);
  st = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADRESS << 1 , EEPROM_MEM_ADRESS + 8, I2C_MEMADD_SIZE_8BIT , (uint8_t*)&in[8], 8 , HAL_MAX_DELAY);
  if (st != HAL_OK) {I2C_Error_Write++;}
  eeprom_wait_ready(&hi2c1, EEPROM_ADRESS, 20);
  st = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADRESS << 1 , EEPROM_MEM_ADRESS + 16, I2C_MEMADD_SIZE_8BIT , (uint8_t*)&iv[0], 8 , HAL_MAX_DELAY);
  if (st != HAL_OK) {I2C_Error_Write++;}
  eeprom_wait_ready(&hi2c1, EEPROM_ADRESS, 20);
  st = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADRESS << 1 , EEPROM_MEM_ADRESS + 24, I2C_MEMADD_SIZE_8BIT , (uint8_t*)&iv[8], 8 , HAL_MAX_DELAY);
  if (st != HAL_OK) {I2C_Error_Write++;}
  eeprom_wait_ready(&hi2c1, EEPROM_ADRESS, 20);
}

void eeprom_get_temperature(int output[3]) {
  uint8_t out[16] = { 0 };
  uint8_t iv[16] = {0};
  st = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADRESS << 1, EEPROM_MEM_ADRESS, I2C_MEMADD_SIZE_8BIT, out, 16, HAL_MAX_DELAY);
  if (st != HAL_OK) {I2C_Error_Read++;}
  eeprom_wait_ready(&hi2c1, EEPROM_ADRESS, 20);
  st = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADRESS << 1, EEPROM_MEM_ADRESS + 16, I2C_MEMADD_SIZE_8BIT, iv, 16, HAL_MAX_DELAY);
  eeprom_wait_ready(&hi2c1, EEPROM_ADRESS, 20);
  struct AES_ctx ctx;
  AES_init_ctx_iv(&ctx, key, iv);
  AES_CBC_decrypt_buffer(&ctx, out, 16);
  if (out[6] == (uint8_t) 0xAB) {
      output[0] = (int) ((int)out[0] - 1) * out[1];
      output[1] = (int) ((int)out[2] - 1) * out[3];
      output[2] = (int) ((int)out[4] - 1) * out[5];
      NotMatchingError = 0;
  } else {
    NotMatchingError = 1;
  }

}

// ---- helpers ----
static HAL_StatusTypeDef eeprom_wait_ready(I2C_HandleTypeDef *hi2c, uint8_t addr7, uint32_t timeout_ms)
{
    uint32_t t0 = HAL_GetTick();
    while ((HAL_GetTick() - t0) < timeout_ms) {
        if (HAL_I2C_IsDeviceReady(hi2c, addr7 << 1, 1, 5) == HAL_OK) return HAL_OK;
    }
    return HAL_TIMEOUT;
}