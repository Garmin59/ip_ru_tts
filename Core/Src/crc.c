/**
  ******************************************************************************
  * @file    crc.c
  * @brief   This file provides code for the configuration
  *          of the CRC instances.
  ******************************************************************************
  */
#include "crc.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_crc.h"


/* CRC init function */
void MX_CRC_Init (void)
{

  /* Peripheral clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);

  LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_NONE);
  LL_CRC_SetOutputDataReverseMode(CRC, LL_CRC_OUTDATA_REVERSE_NONE);
  LL_CRC_SetPolynomialCoef(CRC, LL_CRC_DEFAULT_CRC32_POLY);
  LL_CRC_SetPolynomialSize(CRC, LL_CRC_POLYLENGTH_32B);
  LL_CRC_SetInitialData(CRC, LL_CRC_DEFAULT_CRC_INITVALUE);

}

bool check_crc16_flatcher(const uint8_t *buff, size_t pack_size)
{
    uint8_t crc_a = 0;
    uint8_t crc_b = 0;

    for (int j = 0; j < (pack_size - 2); j++)
    {
        crc_a = crc_a + *(buff + j);
        crc_b = crc_b + crc_a;
    }
    if ((*(buff + pack_size - 2) == crc_a) && (*(buff + pack_size - 1) == crc_b))   {
        return true;
    }
    else   {
        return false;
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */