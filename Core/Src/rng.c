/*****************************************************************************//**
* \file    rng.c
* \brief   This file provides code for the configuration
*          of the RNG instances.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "rng.h"


/** \brief RNG init function */
void rng_init(void)
{
  /* Peripheral clock enable */
  // LL_AHB2_GRP1_EnableClock (LL_AHB2_GRP1_PERIPH_RNG);

  RNG->CR |= RNG_CR_RNGEN;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
