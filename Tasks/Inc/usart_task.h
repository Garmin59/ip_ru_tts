/**************************************************************
 * @file usart_task.h
 * @author Ilya Petrukhin (ilya.petrukhin@gmail.com)
 * @brief 
 * @version 0.1
 * @date 03-08-2023
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************/

#ifndef _USART_TASK_
#define _USART_TASK_

#include <stdint.h>
#include <stdbool.h>

void usart_task (void *param);
bool usart_send (void *data, uint32_t size);  // main send
void uart_print (const char *message);  // debug send

#endif  // _USART_TASK_