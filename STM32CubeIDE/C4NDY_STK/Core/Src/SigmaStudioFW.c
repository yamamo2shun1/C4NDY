/*
 * SigmaStudioFW.c
 *
 *  Created on: Sep 4, 2024
 *      Author: shun
 */
#include "SigmaStudioFW.h"

#include "i2c.h"

void SIGMA_WRITE_REGISTER_BLOCK(uint16_t devAddress, uint16_t address, uint32_t length, uint8_t* pData)
{
    HAL_StatusTypeDef status;
    status = HAL_I2C_Mem_Write(&hi2c2, devAddress, address, I2C_MEMADD_SIZE_16BIT, pData, length, 10000);
    if (status != HAL_OK)
    {
        SEGGER_RTT_printf(0, "[%X] i2c write error\n", address);
    }
}

void SIGMA_SAFELOAD_WRITE_DATA(uint8_t devAddress, uint16_t dataAddress, uint16_t length, uint8_t* pData)
{
    HAL_I2C_Mem_Write(&hi2c2, devAddress, dataAddress, I2C_MEMADD_SIZE_16BIT, pData, length, 10000);
}

void SIGMA_WRITE_DELAY(uint16_t devAddress, uint32_t length, uint8_t* pData)
{
    HAL_Delay(15);
}
