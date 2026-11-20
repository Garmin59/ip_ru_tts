//# @file dac_task.c

#include "dac_task.h"
#include "global.h"
#include "dma.h"

#define TIM15_FREQ    10000     // 10kHz
#define TIM15_KER_CK  40000000  // 40MHz
#define TIM15_ARR     (TIM15_KER_CK / TIM15_FREQ) - 1

// #define DAC_CIRC_BUFFER_SIZE  1024
#define DAC_BUFFER_SIZE       (4096 * 2)
// uint8_t dac_circ_buffer[DAC_CIRC_BUFFER_SIZE];

uint8_t dac_buffer[DAC_BUFFER_SIZE];

void dac_task (void *arg)
{
  // static queue_data dac_data;
  static uint32_t dac_data_size;
  static uint8_t data;

  (void)arg;

  // init GPIO
  // init DAC1
  DBGMCU->APB2FZ = DBGMCU_APB2FZ_DBG_TIM15_STOP;  // for debug purpose
  // dac timer TIM15
  TIM15->CR1 = TIM_CR1_CEN                * 0
              | TIM_CR1_UDIS              * 0
              | TIM_CR1_URS               * 0
              | TIM_CR1_OPM               * 0
              | TIM_CR1_ARPE              * 1
              | TIM_CR1_CKD_X1
              | TIM_CR1_UIFREMAP          * 0
              | TIM_CR1_DITHEN            * 0;
  TIM15->CR2 = TIM_CR2_CCPC               * 0
              | TIM_CR2_CCUS              * 0
              | TIM_CR2_CCDS              * 0
              | TIM_CR2_MMS_RESET
              | TIM_CR2_TI1S              * 0
              | TIM_CR2_OIS1              * 0
              | TIM_CR2_OIS1N             * 0
              | TIM_CR2_OIS2              * 0;
  TIM15->SMCR = 0;
  TIM15->PSC = 0; // 40MHz
  TIM15->ARR = TIM15_ARR;
  TIM15->RCR = 0;
  TIM15->CCMR1 = TIM_CCMR1_CC1S_OUT
              | TIM_CCMR1_OC1FE           * 0
              | TIM_CCMR1_OC1PE           * 0
              | TIM_CCMR1_OC1M_FROZEN
              | TIM_CCMR1_OC1CE           * 0
              | TIM_CCMR1_CC2S_OUT
              | TIM_CCMR1_OC2FE           * 0
              | TIM_CCMR1_OC2PE           * 0
              | TIM_CCMR1_OC2M_FROZEN
              | TIM_CCMR1_OC2CE           * 0;
  TIM15->CCR1 = TIM15_ARR / 2;
  TIM15->EGR = TIM_EGR_UG;    // write values
  TIM15->EGR = TIM_EGR_UG;    // write values
  TIM15->DIER = TIM_DIER_UIE              * 0   /*!<Update interrupt enable */
              | TIM_DIER_CC1IE            * 0   /*!<Capture/Compare 1 interrupt enable */
              | TIM_DIER_CC2IE            * 0   /*!<Capture/Compare 2 interrupt enable */
              | TIM_DIER_COMIE            * 0   /*!<COM interrupt enable */
              | TIM_DIER_BIE              * 0   /*!<Break interrupt enable */
              | TIM_DIER_UDE              * 0   /*!<Update DMA request enable */
              | TIM_DIER_CC2DE            * 0   /*!<Capture/Compare 1 DMA request enable */
              | TIM_DIER_CC1DE            * 0;  /*!<Capture/Compare 2 DMA request enable */

  DAC1->CR = DAC_CR_EN1                   * 0
            | DAC_CR_TEN1                 * 0
            | DAC_CR_TSEL_CH1_TIM15
            | DAC_CR_WAVE1_DIS
            | DAC_CR_MAMP1_255
            | DAC_CR_DMAEN1               * 1
            | DAC_CR_DMAUDRIE1            * 0
            | DAC_CR_EN2                  * 0
            | DAC_CR_TEN2                 * 0
            | DAC_CR_TSEL_CH2_TIM15
            | DAC_CR_WAVE2_DIS
            | DAC_CR_MAMP2_255
            | DAC_CR_DMAEN2               * 1
            | DAC_CR_DMAUDRIE2            * 0;
  DAC1->MCR = DAC_MCR_MODE1_NORM_EXT_BUFF
            | DAC_MCR_DMADOUBLE1          * 0
            | DAC_MCR_SINFORMAT1_SIGNED
            | DAC_MCR_HFSEL_160MHZ
            | DAC_MCR_MODE2_NORM_EXT_BUFF
            | DAC_MCR_DMADOUBLE2          * 0
            | DAC_MCR_SINFORMAT2_SIGNED;
  DAC1->SHSR1 = 0;
  DAC1->SHSR2 = 0;
  DAC1->SHHR = 0;
  DAC1->SHRR = 0;
  // DAC1->CR |= DAC_CR_EN1 | DAC_CR_EN2;
  // TIM15->CR1 |= TIM_CR1_CEN;

  // Enable interrupts
  NVIC_SetPriority (TIM6_DAC_IRQn, (configMAX_PRIORITIES + 2));
  NVIC_EnableIRQ (TIM6_DAC_IRQn);

  // NVIC_SetPriority (TIM1_BRK_TIM15_IRQn, (configMAX_PRIORITIES + 2)); // 
  // NVIC_EnableIRQ (TIM1_BRK_TIM15_IRQn);

  // DMA init
  dma1_ch4_init ();
  dma1_ch5_init ();

  while (1)
  {
    if (xQueueReceive (g.tts_out_queue, &data, portMAX_DELAY)) // wait to get input data
    {
      if (get_circular_buffer (&g.tts_out_buffer, dac_buffer, &dac_data_size))
      {
        dma1_ch4_enable (dac_buffer, dac_data_size);
        dma1_ch5_enable (dac_buffer, dac_data_size);

        DAC1->CR |= DAC_CR_EN1;
        DAC1->CR |= DAC_CR_EN2;
        TIM15->CR1 |= TIM_CR1_CEN;

        xSemaphoreTake (g.dac_end_semaphore, portMAX_DELAY);  // wait to finish DAC output

        DAC1->CR &= ~DAC_CR_EN1;
        DAC1->CR &= ~DAC_CR_EN2;
        TIM15->CR1 &= ~TIM_CR1_CEN;
      }
    }
  }
}



void TIM6_DAC_IRQHandler (void)
{
  if (DAC1->SR & DAC_SR_DMAUDR1)
  {
    DAC1->SR |= DAC_SR_DMAUDR1;
  }
  if (DAC1->SR & DAC_SR_DMAUDR2)
  {
    DAC1->SR |= DAC_SR_DMAUDR2;
  }

}



// void TIM6_DAC_IRQHandler (void)
// {

// }
