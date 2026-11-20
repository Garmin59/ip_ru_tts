/**************************************************************
 * @file usart.c
 * @author Ilya Petrukhin ()
 * @brief 
 * @version 0.1
 * @date 16-04-2025
 * 
 * @copyright Copyright (c) 2025
 * 
**************************************************************/
#include "global.h"
#include "usart.h"
#include "error.h"
#include "stm32g4xx_hal.h"
#include "dma.h"
#include "circular_buffer.h"


// static uint32_t tx_size;      // tx buffer and pointers ISR version
// static uint8_t  *tx_pointer;
// static uint8_t  tx_buffer[UART_BUFFER_SIZE];

// static uint32_t rx_size;      // rx buffer and pointers ISR version
// static uint8_t  *rx_pointer;
static uint8_t  rx_buffer[UART_BUFFER_SIZE];


/**************************************************************
 * @brief setup USART1 115200 8 N 1
 *     PC4     ------> USART1_TX
 *     PC5     ------> USART1_RX
 * 
**************************************************************/
void usart1_init (void)
{
  USART1->CR1 =0;
  USART1->CR1 = USART_CR1_UE                  * 0  /*!< USART Enable */
                | USART_CR1_UESM              * 0  /*!< USART Enable in STOP Mode */
                | USART_CR1_RE                * 1  /*!< Receiver Enable */
                | USART_CR1_TE                * 1  /*!< Transmitter Enable */
                | USART_CR1_IDLEIE            * 0  /*!< IDLE Interrupt Enable */
                | USART_CR1_RXNEIE            * 0  /*!< RXNE Interrupt Enable */
                | USART_CR1_TCIE              * 0  /*!< Transmission Complete Interrupt Enable */
                | USART_CR1_TXEIE             * 0  /*!< TXE Interrupt Enable */
                | USART_CR1_TXEIE_TXFNFIE     * 0  /*!< TXE and TX FIFO Not Full Interrupt Enable */
                | USART_CR1_PEIE              * 0  /*!< PE Interrupt Enable */
                | USART_CR1_PS                * 0  /*!< Parity Selection */
                | USART_CR1_PCE               * 0  /*!< Parity Control Enable */
                | USART_CR1_WAKE              * 0  /*!< Receiver Wakeup method */
                | USART_CR1_M0                * 0  /*!< Word length - Bit 0  (8 bit -> M1=0, M0=0) */
                | USART_CR1_MME               * 0  /*!< Mute Mode Enable */
                | USART_CR1_CMIE              * 0  /*!< Character match interrupt enable */
                | USART_CR1_OVER8             * 0  /*!< Oversampling by 8-bit or 16-bit mode */
                | USART_CR1_DEDT              * 0  /*!< DEDT[4:0] bits (Driver Enable Deassertion Time) */
                | USART_CR1_DEAT              * 0  /*!< DEAT[4:0] bits (Driver Enable Assertion Time) */
                | USART_CR1_RTOIE             * 1  /*!< Receive Time Out interrupt enable */
                | USART_CR1_EOBIE             * 0  /*!< End of Block interrupt enable */
                | USART_CR1_M1                * 0  /*!< Word length - Bit 1 (8 bit -> M1=0, M0=0) */
                | USART_CR1_FIFOEN            * 0  /*!< FIFO mode enable */
                | USART_CR1_TXFEIE            * 0  /*!< TXFIFO empty interrupt enable */
                | USART_CR1_RXFFIE            * 0; /*!< RXFIFO Full interrupt enable */

  USART1->CR2 = USART_CR2_SLVEN               * 0  /*!< Synchronous Slave mode enable */
                | USART_CR2_DIS_NSS           * 0  /*!< Slave Select (NSS) pin management */
                | USART_CR2_ADDM7             * 0  /*!< 7-bit or 4-bit Address Detection */
                | USART_CR2_LBDL              * 0  /*!< LIN Break Detection Length */
                | USART_CR2_LBDIE             * 0  /*!< LIN Break Detection Interrupt Enable */
                | USART_CR2_LBCL              * 0  /*!< Last Bit Clock pulse */
                | USART_CR2_CPHA              * 0  /*!< Clock Phase */
                | USART_CR2_CPOL              * 0  /*!< Clock Polarity */
                | USART_CR2_CLKEN             * 0  /*!< Clock Enable */
                | USART_CR2_STOP_1_0               /*!< STOP bits 1 */
                | USART_CR2_LINEN             * 0  /*!< LIN mode enable */
                | USART_CR2_SWAP              * 0  /*!< SWAP TX/RX pins */
                | USART_CR2_RXINV             * 0  /*!< RX pin active level inversion */
                | USART_CR2_TXINV             * 0  /*!< TX pin active level inversion */
                | USART_CR2_DATAINV           * 0  /*!< Binary data inversion */
                | USART_CR2_MSBFIRST          * 0  /*!< Most Significant Bit First */
                | USART_CR2_ABREN             * 0  /*!< Auto Baud-Rate Enable*/
                | USART_CR2_ABRMODE_55             /*!< ABRMOD[1:0] bits (Auto Baud-Rate Mode) */
                | USART_CR2_RTOEN             * 1  /*!< Receiver Time-Out enable */
                | USART_CR2_ADD               * 0; /*!< Address of the USART node */

  USART1->CR3 = USART_CR3_EIE                 * 0  /*!< Error Interrupt Enable */
                | USART_CR3_IREN              * 0  /*!< IrDA mode Enable */
                | USART_CR3_IRLP              * 0  /*!< IrDA Low-Power */
                | USART_CR3_HDSEL             * 0  /*!< Half-Duplex Selection */
                | USART_CR3_NACK              * 0  /*!< SmartCard NACK enable */
                | USART_CR3_SCEN              * 0  /*!< SmartCard mode enable */
                | USART_CR3_DMAR              * 1  /*!< DMA Enable Receiver */
                | USART_CR3_DMAT              * 1  /*!< DMA Enable Transmitter */
                | USART_CR3_RTSE              * 0  /*!< RTS Enable */
                | USART_CR3_CTSE              * 0  /*!< CTS Enable */
                | USART_CR3_CTSIE             * 0  /*!< CTS Interrupt Enable */
                | USART_CR3_ONEBIT            * 0  /*!< One sample bit method enable */
                | USART_CR3_OVRDIS            * 0  /*!< Overrun Disable */
                | USART_CR3_DDRE              * 0  /*!< DMA Disable on Reception Error */
                | USART_CR3_DEM               * 0  /*!< Driver Enable Mode */
                | USART_CR3_DEP               * 0  /*!< Driver Enable Polarity Selection */
                | USART_CR3_SCARCNT           * 0  /*!< SCARCNT[2:0] bits (SmartCard Auto-Retry Count) */
                | USART_CR3_WUS_START              /*!< WUS[1:0] bits (Wake UP Interrupt Flag Selection) */
                | USART_CR3_WUFIE             * 0  /*!< Wake Up Interrupt Enable */
                | USART_CR3_TXFTIE            * 0  /*!< TXFIFO threshold interrupt enable */
                | USART_CR3_TCBGTIE           * 0  /*!< Transmission Complete Before Guard Time Interrupt Enable */
                | USART_CR3_RXFTCFG_1_8            /*!< RXFIFO FIFO threshold configuration */
                | USART_CR3_RXFTIE            * 0  /*!< RXFIFO threshold interrupt enable */
                | USART_CR3_TXFTCFG_EMPTY        ; /*!< TXFIFO threshold configuration */
  // PCLK2 = 20M, 115200 baud, USARTDIV = 20000000 / 115200 = 174
  USART1->BRR = 174;
  USART1->PRESC = USART_PRESC_PRESCALER1;
  USART1->RTOR = 30 // 30 characters for Receiver Time Out
                | 0 << USART_RTOR_BLEN_Pos;

  // rx_pointer = &rx_buffer[0];
  // tx_pointer = &tx_buffer[0];

  /**************************  DMA TX setup ************************************/
  DMA1->CCR2 = 0;
  DMA1->CCR2 = DMA_CCR_EN               * 0   /*!< Channel enable                      */
              | DMA_CCR_TCIE            * 1   /*!< Transfer complete interrupt enable  */
              | DMA_CCR_HTIE            * 0   /*!< Half Transfer interrupt enable      */
              | DMA_CCR_TEIE            * 1   /*!< Transfer error interrupt enable     */
              | DMA_CCR_DIR_MEM_2_PER
              | DMA_CCR_CIRC            * 0   /*!< Circular mode                       */
              | DMA_CCR_PINC            * 0   /*!< Peripheral increment mode           */
              | DMA_CCR_MINC            * 1   /*!< Memory increment mode               */
              | DMA_CCR_PSIZE_8BITS
              | DMA_CCR_MSIZE_8BITS
              | DMA_CCR_PL_MEDIUM
              | DMA_CCR_MEM2MEM         * 0;  /*!< Memory to memory mode               */
  // DMA1->CMAR2 = (uint32_t)tx_buffer;
  DMA1->CPAR2 = (uint32_t)&USART1->TDR;
  DMA1->CNDTR2 = (uint32_t)UART_BUFFER_SIZE;

  // DMAMUX1_Channel1->CCR &= ~DMAMUX_CxCR_DMAREQ_ID_Msk;
  DMAMUX1_Channel1->CCR = DMAMUX_CxCR_DMAREQ_USART1_TX;

  /*************************** DMA RX setup *****************************************/
  DMA1->CCR3 = 0;
  DMA1->CCR3 = DMA_CCR_EN               * 0   /*!< Channel enable                      */
              | DMA_CCR_TCIE            * 1   /*!< Transfer complete interrupt enable  */
              | DMA_CCR_HTIE            * 0   /*!< Half Transfer interrupt enable      */
              | DMA_CCR_TEIE            * 1   /*!< Transfer error interrupt enable     */
              | DMA_CCR_DIR_PER_2_MEM
              | DMA_CCR_CIRC            * 0   /*!< Circular mode                       */
              | DMA_CCR_PINC            * 0   /*!< Peripheral increment mode           */
              | DMA_CCR_MINC            * 1   /*!< Memory increment mode               */
              | DMA_CCR_PSIZE_8BITS
              | DMA_CCR_MSIZE_8BITS
              | DMA_CCR_PL_MEDIUM
              | DMA_CCR_MEM2MEM         * 0;  /*!< Memory to memory mode               */
  DMA1->CMAR3 = (uint32_t)&rx_buffer[0];
  DMA1->CPAR3 = (uint32_t)&USART1->RDR;
  DMA1->CNDTR3 = (uint32_t)UART_BUFFER_SIZE;

  DMAMUX1_Channel2->CCR = DMAMUX_CxCR_DMAREQ_USART1_RX;

  // DMA1->CCR3 |= DMA_CCR_EN;

  // Enable interrupts
  NVIC_SetPriority (DMA1_Channel2_IRQn, (configMAX_PRIORITIES + 2)); // 
  NVIC_EnableIRQ (DMA1_Channel2_IRQn);

  NVIC_SetPriority (DMA1_Channel3_IRQn, (configMAX_PRIORITIES + 2)); // 
  NVIC_EnableIRQ (DMA1_Channel3_IRQn);

  NVIC_SetPriority (USART1_IRQn, (configMAX_PRIORITIES + 2)); // 
  NVIC_EnableIRQ (USART1_IRQn);


  DMA1->CCR3 |= DMA_CCR_EN;     // enable rx dma
  USART1->CR1 |= USART_CR1_UE;  // enable USART

  xSemaphoreGive (g.usart_tx_semaphore);  // release semaphore

}

/**************************************************************
 * @brief 
 * 
**************************************************************/
void USART1_IRQHandler (void)
{
  // BaseType_t taskwoken_tx;
  // BaseType_t taskwoken;
  uint32_t reg = USART1->ISR;
  volatile uint32_t size;

  // TX ISR version
  // if (reg & USART_ISR_TXE)  // transmit data register empty
  // {
  //   if (tx_size)  // transmit continue
  //   {
  //     USART1->TDR = *tx_pointer++;
  //     tx_size--;
  //   } 
  //   else // finish transmit
  //   {
  //     USART1->CR1 &= ~USART_CR1_TXEIE;
  //     // release tx_semaphore
  //     xSemaphoreGiveFromISR (g.usart_tx_semaphore, &taskwoken_tx);
  //     if (taskwoken_tx)
  //     {
  //       portYIELD ();
  //     }
  //   } 
  // }
  // RX ISR version
  // if (reg & USART_ISR_RXNE)
  // {
  //   *rx_pointer++ = USART1->RDR;
  //   if (rx_pointer >= &rx_buffer[UART_BUFFER_SIZE])
  //   {
  //     // send rx message by full buffer
  //     put_circular_buffer (&g.usart_rx_buffer, &rx_buffer[0], UART_BUFFER_SIZE);
  //     rx_pointer = &rx_buffer[0];
  //     xSemaphoreGiveFromISR (g.usart_rx_semaphore, &taskwoken_rx);
  //     if (taskwoken_rx)
  //     {
  //       portYIELD ();
  //     }
  //   }
  // }

  // RX ISR version
  // if (reg & USART_ISR_RTOF)  // receive time out
  // {
  //   USART1->ICR = USART_ICR_RTOCF;
  //   // send rx message by receive timeout
  //   uint32_t size = (uint32_t)rx_pointer - (uint32_t)&rx_buffer[0];
  //   put_circular_buffer (&g.usart_rx_buffer, &rx_buffer[0], size);
  //   rx_pointer = &rx_buffer[0];
  //   xSemaphoreGiveFromISR (g.usart_rx_semaphore, &taskwoken_rx);
  //   if (taskwoken_rx)
  //   {
  //     portYIELD ();
  //   }
  // }

  // DMA version
  if (reg & USART_ISR_RTOF)  // receive time out
  {
    USART1->ICR = USART_ICR_RTOCF;
    // send rx message by receive timeout
    size = (uint32_t)UART_BUFFER_SIZE - DMA1->CNDTR3;
    put_circular_buffer (&g.usart_rx_buffer, &rx_buffer[0], size);
    // setup DMA
    DMA1->CCR3 &= ~DMA_CCR_EN;
    DMA1->CMAR3 =(uint32_t)&rx_buffer[0];
    DMA1->CNDTR3 = (uint32_t)UART_BUFFER_SIZE;
    DMA1->CCR3 |= DMA_CCR_EN;
    // xSemaphoreGiveFromISR (g.usart_rx_semaphore, &taskwoken);
    // if (taskwoken)
    // {
    //   portYIELD ();
    // }
  }
  
}

/**************************************************************
 * @brief 
 * 
 * @param data 
 * @param size 
 * @return true 
 * @return false 
**************************************************************/
bool usart1_send (uint8_t *data, uint32_t size)
{
  if (size <= UART_BUFFER_SIZE)
  {
    DMA1->CMAR2 = (uint32_t)data;
    DMA1->CNDTR2 = size;
    DMA1->IFCR = DMA_IFCR_CGIF2;
    DMA1->CCR2 |= DMA_CCR_EN;
  }
  return true;
}

/**************************************************************
 * @brief interrupt DMA1 CH2 support USART1 TX
 * 
**************************************************************/
void DMA1_Channel2_IRQHandler (void)
{
  BaseType_t taskwoken;

  uint32_t reg = DMA1->ISR;
  if (reg & DMA_ISR_TCIF2)  // transfer complete
  {
    DMA1->IFCR = DMA_IFCR_CTCIF2;
    DMA1->CCR2 &= ~DMA_CCR_EN;
    // release tx_semaphore
    xSemaphoreGiveFromISR (g.usart_tx_semaphore, &taskwoken);
    if (taskwoken)
    {
      portYIELD ();
    }
  }
  // if (reg & DMA_ISR_TEIF2)  // error
  // {
  //   DMA1->IFCR = DMA_IFCR_CTEIF2; // for debug purpose
  // }
  // clear all flags
  DMA1->IFCR = DMA_IFCR_CGIF2;
}


/**************************************************************
 * @brief interrupt DMA1 CH3 support USART1 RX
 * 
**************************************************************/
void DMA1_Channel3_IRQHandler (void)
{
  BaseType_t taskwoken;

  uint32_t reg = DMA1->ISR;
  if (reg & DMA_ISR_TCIF3)  // transfer complete
  {
    DMA1->IFCR = DMA_IFCR_CTCIF3;
    // send rx message by full buffer
    put_circular_buffer (&g.usart_rx_buffer, &rx_buffer[0], UART_BUFFER_SIZE);
    // setup DMA
    DMA1->CCR3 &= ~DMA_CCR_EN;
    DMA1->CMAR3 = (uint32_t)&rx_buffer[0];
    DMA1->CNDTR3 = (uint32_t)UART_BUFFER_SIZE;
    DMA1->CCR3 |= DMA_CCR_EN;
    // release rx_semaphore
    xSemaphoreGiveFromISR (g.usart_rx_semaphore, &taskwoken);
    if (taskwoken)
    {
      portYIELD ();
    }
  }
  // if (reg & DMA_ISR_TEIF3)  // error
  // {
  //   DMA1->IFCR = DMA_IFCR_CTEIF3; // for debug purpose
  // }
  // clear all flags
  DMA1->IFCR = DMA_IFCR_CGIF3;
}


