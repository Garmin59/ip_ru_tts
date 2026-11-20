/**************************************************************
 * @brief Usage resources:
 * SysTick timer 1ms
 * 
 * Memcpy_dma: DMA1_CH1
 * USART1: DMA1_CH2 - tx, DMA1_CH3 - rx, 115200 8N1
 * SPI2: DMA1_CH4 - tx, DMA1_CH5 - rx, TIM7 - start
 * 
**************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "global.h"
#include "gpio.h"
#include "crc.h"
#include "usart.h"
#include "error.h"
#include "GlobalConfig.h"
#include "usart_task.h"
#include "test_task.h"
#include "imu_task.h"
#include "button_task.h"
#include "tts_task.h"
#include "dac_task.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#define KBYTE_SIZE          256             // 256 * 4byte = 1024

#define INIT_STACK_SIZE     2 * KBYTE_SIZE  // Init task stack size

#define UART_STACK_SIZE     1 * KBYTE_SIZE
#define TEST_STACK_SIZE     2 * KBYTE_SIZE
#define IMU_STACK_SIZE      2 * KBYTE_SIZE
#define BUTTON_STACK_SIZE   1 * KBYTE_SIZE
#define TTS_STACK_SIZE      8 * KBYTE_SIZE
#define DAC_STACK_SIZE      2 * KBYTE_SIZE

#define TTS_QUEUE_LENGTH  1
#define TTS_OUT_QUEUE_LENGTH  50
#define DAC_QUEUE_LENGTH  100

Global	g = 
{

};



static StackType_t  init_stack_buffer[INIT_STACK_SIZE];
static StaticTask_t init_task_tcb;
static StackType_t  uart_stack_buffer[UART_STACK_SIZE];
static StaticTask_t uart_task_tcb;
static StackType_t  test_stack_buffer[TEST_STACK_SIZE];
static StaticTask_t test_task_tcb;
static StackType_t  tts_stack_buffer[TTS_STACK_SIZE];
static StaticTask_t tts_task_tcb;
static StackType_t  dac_stack_buffer[TTS_STACK_SIZE];
static StaticTask_t dac_task_tcb;

static StackType_t  button_stack_buffer[BUTTON_STACK_SIZE];
static StaticTask_t button_task_tcb;


static StaticSemaphore_t  usart_tx_semaphore_buffer;
static StaticSemaphore_t  usart_rx_semaphore_buffer;
static StaticSemaphore_t  dac_end_semaphore_buffer;

static StaticQueue_t tts_QueueBuffer;
static StaticQueue_t tts_out_QueueBuffer;
static uint8_t tts_QueueStorage [TTS_QUEUE_LENGTH * sizeof (queue_data) ];
static uint8_t tts_out_QueueStorage [TTS_OUT_QUEUE_LENGTH * sizeof (tts_out_qdata) ];

static StaticEventGroup_t event_group_buffer;

__attribute__((aligned(4)))
static uint8_t uart_tx[UART_BUFFER_SIZE];
__attribute__((aligned(4)))
static uint8_t uart_rx[UART_BUFFER_SIZE];

// static StaticQueue_t dac_QueueBuffer;
static uint8_t tts_out [TTS_OUT_BUFFER_SIZE];

void init_task      (void *param);

char ubuf[512]; // global uart printf buffer


#define DEAD_BEEF 0xDEADBEEF


#ifdef SWO_DEBUG
int _write (int file, char *ptr, int len)
{
    int i = 0;

    for (i = 0; i < len; i++)
    {
        ITM_SendChar (*ptr++);
    }
    return len;
}
#endif

/******************************************************************************
  * @brief  The application entry point.
  * @retval int
  ****************************************************************************/
int main(void)
{
  /* Configure the system clock */
  system_clock_config ();  
  periferals_clocks_enable ();
  all_GPIO_init ();
  SysTick_Config (160000000 / 1000U);
  
  g.usart_tx_semaphore  = xSemaphoreCreateBinaryStatic (&usart_tx_semaphore_buffer);
  g.usart_rx_semaphore  = xSemaphoreCreateBinaryStatic (&usart_rx_semaphore_buffer);

  g.tts_queue = xQueueCreateStatic (TTS_QUEUE_LENGTH, sizeof (queue_data), &tts_QueueStorage[0], &tts_QueueBuffer);
  g.tts_out_queue = xQueueCreateStatic (TTS_OUT_QUEUE_LENGTH, sizeof (tts_out_qdata), &tts_out_QueueStorage[0], &tts_out_QueueBuffer);
  g.dac_end_semaphore = xSemaphoreCreateBinaryStatic (&dac_end_semaphore_buffer);

  g.events = xEventGroupCreateStatic (&event_group_buffer);

  init_circular_buffer (&g.usart_tx_buffer, uart_tx, sizeof (uart_tx));
  init_circular_buffer (&g.usart_rx_buffer, uart_rx, sizeof (uart_rx));
  init_circular_buffer (&g.tts_out_buffer, tts_out, sizeof (tts_out));

  // g.start_task_handle =  xTaskCreateStatic (init_task,     "INIT", INIT_STACK_SIZE,    NULL,   tskIDLE_PRIORITY, init_stack_buffer,  &init_task_tcb);
  // tasks creation
  g.dac_task_handle         = xTaskCreateStatic (dac_task,        "DAC",    DAC_STACK_SIZE,     NULL,   tskIDLE_PRIORITY + 2, dac_stack_buffer,     &dac_task_tcb);
  g.tts_task_handle         = xTaskCreateStatic (tts_task,        "TTS",    TTS_STACK_SIZE,     NULL,   tskIDLE_PRIORITY + 1, tts_stack_buffer,     &tts_task_tcb   );
  g.test_task_handle        = xTaskCreateStatic (test_task,       "TEST",   TEST_STACK_SIZE,    NULL,   tskIDLE_PRIORITY + 0, test_stack_buffer,    &test_task_tcb  );
  g.usart_task_handle       = xTaskCreateStatic (usart_task,      "UART",   UART_STACK_SIZE,    NULL,   tskIDLE_PRIORITY + 0, uart_stack_buffer,    &uart_task_tcb  );
  g.btn_task_handle         = xTaskCreateStatic (button_task,     "BTN",    BUTTON_STACK_SIZE,  NULL,   tskIDLE_PRIORITY + 0, button_stack_buffer,  &button_task_tcb);

  vTaskStartScheduler ();
  /* We should never get here as control is now taken by the scheduler */
  while (1)
  {
  }
}


/**************************************************************
 * @brief initialisation all tasks and peryphery
 * 
 * @param param 
**************************************************************/
void init_task (void *param)
{
  (void)param;

  // tasks creation
  g.dac_task_handle         = xTaskCreateStatic (dac_task,        "DAC",    DAC_STACK_SIZE,     NULL,   tskIDLE_PRIORITY + 2, dac_stack_buffer,     &dac_task_tcb);
  g.tts_task_handle         = xTaskCreateStatic (tts_task,        "TTS",    TTS_STACK_SIZE,     NULL,   tskIDLE_PRIORITY + 1, tts_stack_buffer,     &tts_task_tcb   );
  g.test_task_handle        = xTaskCreateStatic (test_task,       "TEST",   TEST_STACK_SIZE,    NULL,   tskIDLE_PRIORITY + 0, test_stack_buffer,    &test_task_tcb  );
  g.usart_task_handle       = xTaskCreateStatic (usart_task,      "UART",   UART_STACK_SIZE,    NULL,   tskIDLE_PRIORITY + 0, uart_stack_buffer,    &uart_task_tcb  );
  g.btn_task_handle         = xTaskCreateStatic (button_task,     "BTN",    BUTTON_STACK_SIZE,  NULL,   tskIDLE_PRIORITY + 0, button_stack_buffer,  &button_task_tcb);

  // LED_GREEN_OFF;

  while (1)
  {
      vTaskDelay (pdMS_TO_TICKS (1500));
      // LED_GREEN_TOGGLE;
  }

  vTaskDelete (NULL);
}

/*****************************************************************************//**
* \brief  This function is executed in case of error occurrence.
* \retval None
*******************************************************************************/
void Error_Handler (void)
{
  __disable_irq ();
  while (1)
  {
  }
}


#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed (uint8_t *file, uint32_t line)
{
  printf ("Wrong parameters value: file %s on line %d\r\n", file, line);
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */
