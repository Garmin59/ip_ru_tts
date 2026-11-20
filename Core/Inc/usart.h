/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdarg.h>
#include "stm32g4xx_hal.h"
#include "global.h"

extern char ubuf[];
extern UART_HandleTypeDef huart2;

void MX_USART2_UART_Init(void);

void usart1_init (void);
bool usart1_send (uint8_t *data, uint32_t size);
bool usart_send (void *data, uint32_t size);


#define UART_LOG(fmt, ...) do {	\
              uint32_t ulen = snprintf (ubuf, UART_BUFFER_SIZE, fmt, ##__VA_ARGS__); \
              usart_send (ubuf, ulen); \
            } while (0)

            /* #define UART_LOG(fmt, ...) do { \
            sprintf (ubuf, "Time: %d ", (int)xTaskGetTickCount ());   \
            sprintf (&ubuf[strlen(ubuf)], fmt, ##__VA_ARGS__);        \
            usart_send ((uint8_t*)ubuf, strlen(ubuf));					      \
            } while (0) 
            */

#ifdef __cplusplus
}
#endif

