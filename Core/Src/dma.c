/**************************************************************
 * @file dma.c
 * @author Ilya Petrukhin (ilya.petrukhin@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-08-2023
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************/

#include "global.h"
#include "dma.h"

/********************** DMA channel usage *******************************
* DMA1 channel 1  - memory to memory memcpy_dma
* DMA1 channel 2  - usart1 tx support
* DMA1 channel 3  - usart1 rx support
* DMA1 channel 4  - DAC1 ch1 
* DMA1 channel 5  - DAC1 ch2 
*
* DMAMUX channel 1  - mux usart1 tx
* DMAMUX channel 2  - mux usart1 rx
* DMAMUX channel 4  - mux dma1 ch1
* DMAMUX channel 5  - mux dma1 ch2
*
***********************************************************************/

/**************************************************************
 * @brief memory to memory copy with DMA support
 * 
 * @param dest    destination
 * @param src     source
 * @param size    size
 * @return void*  pointer to the next of destination
**************************************************************/
void *memcpy_dma (void *dest, void *src, uint16_t size)
{
  // 7. Настраиваем регистр управления CCR:
  //    - MEM2M: включение режима Memory-to-Memory (бит 14)
  //    - DIR: направление 1 = с периферии в память (в M2M это означает CPAR -> CMAR)
  //    - PINC: инкремент адреса источника (CPAR)
  //    - MINC: инкремент адреса назначения (CMAR)
  //    - PSIZE / MSIZE: размер данных (00 = 8-bit, 01 = 16-bit, 10 = 32-bit)
  //    - PL: приоритет (например, Medium = 01)
  //    - CIRC: выключено (Memory-to-Memory не должно работать в цикле)

  DMA1->CCR1 &= ~DMA_CCR_EN;
  // Ждем, пока бит EN действительно сбросится
  while (DMA1->CCR1 & DMA_CCR_EN);
  // 3. Очищаем флаги прерываний для Channel 1
  DMA1->IFCR = DMA_IFCR_CGIF1;
  DMA1->CCR1 = DMA_CCR_EN               * 0   /*!< Channel enable                      */
              | DMA_CCR_TCIE            * 0   /*!< Transfer complete interrupt enable  */
              | DMA_CCR_HTIE            * 0   /*!< Half Transfer interrupt enable      */
              | DMA_CCR_TEIE            * 0   /*!< Transfer error interrupt enable     */
              | DMA_CCR_DIR_PER_2_MEM
              | DMA_CCR_CIRC            * 0   /*!< Circular mode                       */
              | DMA_CCR_PINC            * 1   /*!< Peripheral increment mode           */
              | DMA_CCR_MINC            * 1   /*!< Memory increment mode               */
              | DMA_CCR_PSIZE_8BITS
              | DMA_CCR_MSIZE_8BITS
              | DMA_CCR_PL_MEDIUM
              | DMA_CCR_MEM2MEM         * 1;  /*!< Memory to memory mode               */

  DMA1->CPAR1 = (uint32_t)src;
  DMA1->CMAR1 = (uint32_t)dest;
  DMA1->CNDTR1 = (uint32_t)size;
  DMA1->IFCR = DMA_ISR_GIF1;
  // Сбрасываем флаг завершения передачи (TCIF)
  DMA1->IFCR = DMA_IFCR_CTCIF1;
  DMA1->CCR1 |= DMA_CCR_EN;

  // Ожидаем завершения передачи (флаг TCIF1 в регистре ISR)
  while (!(DMA1->ISR & DMA_ISR_TCIF1));
  // Отключаем канал после завершения
  DMA1->CCR1 &= ~DMA_CCR_EN;

  return (void *)((uint32_t)dest + (uint32_t)size);
}


/**************************************************************
 * @brief init DMA TX before USART1
 * 
 * @param tx_buffer 
 * @param tx_size 
**************************************************************/
void usart1_tx_dma_init (void *tx_buffer, uint16_t tx_size)
{
  DMA1->CCR2 = DMA_CCR_EN               * 0   /*!< Channel enable                      */
              | DMA_CCR_TCIE            * 0   /*!< Transfer complete interrupt enable  */
              | DMA_CCR_HTIE            * 0   /*!< Half Transfer interrupt enable      */
              | DMA_CCR_TEIE            * 0   /*!< Transfer error interrupt enable     */
              | DMA_CCR_DIR_MEM_2_PER
              | DMA_CCR_CIRC            * 0   /*!< Circular mode                       */
              | DMA_CCR_PINC            * 0   /*!< Peripheral increment mode           */
              | DMA_CCR_MINC            * 1   /*!< Memory increment mode               */
              | DMA_CCR_PSIZE_8BITS
              | DMA_CCR_MSIZE_8BITS
              | DMA_CCR_PL_MEDIUM
              | DMA_CCR_MEM2MEM         * 0;  /*!< Memory to memory mode               */
  DMA1->CMAR2 = (uint32_t)tx_buffer;
  DMA1->CPAR2 = (uint32_t)&USART1->TDR;
  DMA1->CNDTR2 = (uint32_t)tx_size;

  // DMAMUX1_Channel1->CCR &= ~DMAMUX_CxCR_DMAREQ_ID_Msk;
  DMAMUX1_Channel1->CCR = DMAMUX_CxCR_DMAREQ_USART1_TX;
  // DMAMUX_CxCR_SOIE
  // DMAMUX_CxCR_EGE
  // DMAMUX_CxCR_SE
  // DMAMUX_CxCR_SPOL_RISING
  // (tx_size & 0x1F) << DMAMUX_CxCR_NBREQ_Pos

  // DMA1->CCR2 |= DMA_CCR_EN;

}

/**************************************************************
 * @brief init DMA RX before USART1
 * 
 * @param rx_buffer 
 * @param rx_size 
**************************************************************/
void usart1_rx_dma_init (void *rx_buffer, uint16_t rx_buffer_size)
{
  DMA1->CCR3 = DMA_CCR_EN               * 0   /*!< Channel enable                      */
              | DMA_CCR_TCIE            * 0   /*!< Transfer complete interrupt enable  */
              | DMA_CCR_HTIE            * 0   /*!< Half Transfer interrupt enable      */
              | DMA_CCR_TEIE            * 0   /*!< Transfer error interrupt enable     */
              | DMA_CCR_DIR_PER_2_MEM
              | DMA_CCR_CIRC            * 0   /*!< Circular mode                       */
              | DMA_CCR_PINC            * 0   /*!< Peripheral increment mode           */
              | DMA_CCR_MINC            * 1   /*!< Memory increment mode               */
              | DMA_CCR_PSIZE_8BITS
              | DMA_CCR_MSIZE_8BITS
              | DMA_CCR_PL_MEDIUM
              | DMA_CCR_MEM2MEM         * 0;  /*!< Memory to memory mode               */
  DMA1->CMAR3 = (uint32_t)rx_buffer;
  DMA1->CPAR3 = (uint32_t)&USART1->RDR;
  DMA1->CNDTR3 = (uint32_t)rx_buffer_size;

  DMAMUX1_Channel2->CCR = DMAMUX_CxCR_DMAREQ_USART1_RX;

  // DMA1->CCR3 |= DMA_CCR_EN;

}

/**************************************************************
 * @brief init DMA DAC1 CH1 
 * 
 * @param tx_buffer 
 * @param tx_size 
**************************************************************/
void dma1_ch4_init (void)
{
  DMA1->CCR4 = DMA_CCR_EN               * 0   /*!< Channel enable                      */
              | DMA_CCR_TCIE            * 1   /*!< Transfer complete interrupt enable  */
              | DMA_CCR_HTIE            * 0   /*!< Half Transfer interrupt enable      */
              | DMA_CCR_TEIE            * 0   /*!< Transfer error interrupt enable     */
              | DMA_CCR_DIR_MEM_2_PER
              | DMA_CCR_CIRC            * 0   /*!< Circular mode                       */
              | DMA_CCR_PINC            * 0   /*!< Peripheral increment mode           */
              | DMA_CCR_MINC            * 1   /*!< Memory increment mode               */
              | DMA_CCR_PSIZE_16BITS
              | DMA_CCR_MSIZE_8BITS
              | DMA_CCR_PL_MEDIUM             /*!< Channel Priority level */
              | DMA_CCR_MEM2MEM         * 0;  /*!< Memory to memory mode               */

  DMA1->CPAR4 = (uint32_t)&DAC1->DHR8R1;

  DMAMUX1_Channel4->CCR = DMAMUX_CxCR_DMAREQ_DAC1_CH1
              | DMAMUX_CxCR_SOIE        * 0
              | DMAMUX_CxCR_EGE         * 0
              | DMAMUX_CxCR_SE          * 0
              | DMAMUX_CxCR_SPOL_NONE
              | DMAMUX_CxCR_NBREQ       * 0
              | DMAMUX_CxCR_SYNC_EXTI_LINE0;

  NVIC_SetPriority (DMA1_Channel4_IRQn, (configMAX_PRIORITIES + 2));
  NVIC_EnableIRQ (DMA1_Channel4_IRQn);
}

void dma1_ch4_enable (void *buffer, uint16_t size)
{
  uint8_t *buffer8 = (uint8_t *)buffer;

  DMA1->CMAR4 = (uint32_t)buffer8;
  // DMA1->CPAR4 = (uint32_t)&DAC1->DHR8R1;
  DMA1->CNDTR4 = (uint32_t)size;

  DMA1->CCR4 |= DMA_CCR_EN;  
}


/**************************************************************
 * @brief init DMA DAC1 CH2 
 * 
 * @param tx_buffer 
 * @param tx_size 
**************************************************************/
void dma1_ch5_init (void)
{
  DMA1->CCR5 = DMA_CCR_EN               * 0   /*!< Channel enable                      */
              | DMA_CCR_TCIE            * 1   /*!< Transfer complete interrupt enable  */
              | DMA_CCR_HTIE            * 0   /*!< Half Transfer interrupt enable      */
              | DMA_CCR_TEIE            * 0   /*!< Transfer error interrupt enable     */
              | DMA_CCR_DIR_MEM_2_PER
              | DMA_CCR_CIRC            * 0   /*!< Circular mode                       */
              | DMA_CCR_PINC            * 0   /*!< Peripheral increment mode           */
              | DMA_CCR_MINC            * 1   /*!< Memory increment mode               */
              | DMA_CCR_PSIZE_16BITS
              | DMA_CCR_MSIZE_8BITS
              | DMA_CCR_PL_MEDIUM
              | DMA_CCR_MEM2MEM         * 0;  /*!< Memory to memory mode               */

  DMA1->CPAR5 = (uint32_t)&DAC1->DHR8R2;

  DMAMUX1_Channel5->CCR = DMAMUX_CxCR_DMAREQ_DAC1_CH2
              | DMAMUX_CxCR_SOIE        * 0
              | DMAMUX_CxCR_EGE         * 0
              | DMAMUX_CxCR_SE          * 0
              | DMAMUX_CxCR_SPOL_NONE
              | DMAMUX_CxCR_NBREQ       * 0
              | DMAMUX_CxCR_SYNC_EXTI_LINE0;
  
  NVIC_SetPriority (DMA1_Channel5_IRQn, (configMAX_PRIORITIES + 2));
  NVIC_EnableIRQ (DMA1_Channel5_IRQn);
}

void dma1_ch5_enable (void *buffer, uint16_t size)
{
  uint8_t *buffer8 = (uint8_t *)buffer;

  DMA1->CMAR5 = (uint32_t)buffer8;
  DMA1->CPAR5 = (uint32_t)&DAC1->DHR8R2;
  DMA1->CNDTR5 = (uint32_t)size;

  DMA1->CCR5 |= DMA_CCR_EN;

}


void DMA1_Channel4_IRQHandler (void)
{
  static BaseType_t woken;

  if (DMA1->ISR & DMA_ISR_TCIF4)
  {
    DMA1->IFCR = DMA_IFCR_CTCIF4;
    woken = pdFALSE;
    xSemaphoreGiveFromISR (g.dac_end_semaphore, &woken);
    if (woken)
    {
      portYIELD ();
    }
  }
}

void DMA1_Channel5_IRQHandler (void)
{
  static BaseType_t woken;

  if (DMA1->ISR & DMA_ISR_TCIF5)
  {
    DMA1->IFCR = DMA_IFCR_CTCIF5;
    woken = pdFALSE;
    xSemaphoreGiveFromISR (g.dac_end_semaphore, &woken);
    if (woken)
    {
      portYIELD ();
    }
  }
}


