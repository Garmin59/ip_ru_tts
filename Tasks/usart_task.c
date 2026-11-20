/**************************************************************
 * @file usart_task.c
 * @author Ilya Petrukhin (ilya.petrukhin@gmail.com)
 * @brief 
 * @version 0.1
 * @date 03-08-2023
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************/

#include "usart_task.h"
#include "circular_buffer.h"
#include "usart.h"
#include "global.h"


/**************************************************************
 * @brief task to control usart messages
 * 
 * @param param 
**************************************************************/
void usart_task (void *param)
{
  (void)param;

  static uint8_t buffer[UART_BUFFER_SIZE];
  static uint32_t size;

  usart1_init ();

  while (1)
  {
    // ulTaskNotifyTake (pdTRUE, portMAX_DELAY);

    // Check the message to uart
    if (is_circular_buffer_have_message (&g.usart_tx_buffer))
    {
      // wait to release semaphore
      if (xSemaphoreTake (g.usart_tx_semaphore, pdTICKS_TO_MS (0)) == pdTRUE)
      {
        get_circular_buffer (&g.usart_tx_buffer, buffer, &size);
        // send message to usart1
        usart1_send (buffer, size);
      }
    }

    if (is_circular_buffer_have_message (&g.usart_rx_buffer))
    {
      get_circular_buffer (&g.usart_rx_buffer, buffer, &size);
      /// @todo put command to parsing
    }

    vTaskDelay (pdMS_TO_TICKS(10));

  }
}

/**************************************************************
 * @brief interface function
 * 
 * @param data pointer to the data
 * @param size size of message
**************************************************************/
bool usart_send (void *data, uint32_t size)
{
  return put_circular_buffer (&g.usart_tx_buffer, (uint8_t *)data, size);
}

/**************************************************************
 * @brief interface function
 * 
 * @param string 
**************************************************************/
void uart_print (const char *string)
{
  char buf[100];
  sprintf (buf, string);
  usart_send ((uint8_t *)&buf[0], strlen(buf));
}

