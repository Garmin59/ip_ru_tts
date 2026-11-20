/*****************************************************************************//**
* \file    fdcan.h
* \brief   This file contains all the function prototypes for
*          the fdcan.c file
******************************************************************************
* \attention
*
* <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
* All rights reserved.</center></h2>
*
* This software component is licensed by ST under Ultimate Liberty license
* SLA0044, the "License"; You may not use this file except in compliance with
* the License. You may obtain a copy of the License at:
*                             www.st.com/SLA0044
*
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;

void MX_FDCAN1_Init(void);
void MX_FDCAN2_Init(void);

void CO_filter_config(FDCAN_HandleTypeDef *hfdcan);
void asb_filter_config(FDCAN_HandleTypeDef *hfdcan);
void J1939_filter_config(FDCAN_HandleTypeDef *hfdcan);

#define CAN_TEST_ID_0 0x01
#define CAN_TEST_ID_1 0x02
#define CAN_TEST_ID_2 0x03
#define CAN_TEST_ID_3 0x04

/* To all received messages define individual filter */
#define J1939_INDIVIDUAL_FILTERS 1

/* Тестовый режим, без трансивера */
//#define CAN_TESTMODE 1
#define CAN_ENABLE_FILTERS 1


#define CAN_PRESCALER   6
#define CAN_SYNC_JW     2
#define CAN_SYNC_SEG    1
#define CAN_PHASE_SEG1 11
#define CAN_PHASE_SEG2  4



#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
