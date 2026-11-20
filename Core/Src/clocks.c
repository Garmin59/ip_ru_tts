/**
* \file     clocks.c
* \author   Ilya Petrukhin
* \brief    Clock configuration and re-configuration
*/


#include	"global.h"


/*****************************************************************************//**
* \brief  System Clock Configuration
* \return None
*******************************************************************************/
void system_clock_config (void)
{

#define RCC_PLLM_DIV    RCC_PLLCFGR_PLLM_DIV_3    // HSE (24MHZ) divider 
#define RCC_PLLN_MUL    (uint32_t)(40U)           // PLL mul 8MHz * 40 = 320MHz
#define RCC_PLLQ_DIV    RCC_PLLCFGR_PLLQ_DIV_4    // USB devider 320/4 = 80MHz
#define RCC_PLLR_DIV    RCC_PLLCFGR_PLLR_DIV_2    // SYSCLK devider 320MHz/2 = 160MHz
#define RCC_PLLP_DIV    RCC_PLLCFGR_PLLP_DIV_16   // ADC_CLK devider  320MHz/16= 20MHz
  
  // SCB->VTOR = FLASH_BASE_ADDRESS + FLASH_LOW_APP_OFFSET + FLASH_HEAD_OFFSET; // Set the vector table to the start of app region
  
  FLASH->ACR = FLASH_ACR_LATENCY_4WS
              | FLASH_ACR_PRFTEN   * 1  // 
              | FLASH_ACR_ICEN     * 1  // 
              | FLASH_ACR_DCEN     * 1  // 
              | FLASH_ACR_ICRST    * 0  // 
              | FLASH_ACR_DCRST    * 0  // 
              | FLASH_ACR_RUN_PD   * 0  // 
              | FLASH_ACR_SLEEP_PD * 0  // 
              | FLASH_ACR_DBG_SWEN * 1; // 
  __DSB();
  while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_4WS)
  {
  }
  PWR->CR5 = PWR_CR5_R1MODE * 0;
  PWR->CR3 |= PWR_CR3_UCPD_DBDIS;

  RCC->CR = RCC_CR_HSION        * 1   // 
            | RCC_CR_HSIKERON   * 0   // 
            | RCC_CR_HSEON      * 1   // 
            | RCC_CR_HSEBYP     * 0   // 
            | RCC_CR_CSSON      * 0   // 
            | RCC_CR_PLLON      * 0   // 
            | RCC_CR_DEFAULT_LSB;     //

   /* Wait till HSE is ready */
  while (!(RCC->CR & RCC_CR_HSERDY))
  {
  }
  RCC->CRRCR |= RCC_CRRCR_HSI48ON;
   /* Wait till HSI48 is ready */
  while (!(RCC->CRRCR & RCC_CRRCR_HSI48RDY))
  {
  }

  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE
                | RCC_PLLM_DIV
                | ((RCC_PLLN_MUL << RCC_PLLCFGR_PLLN_Pos) & RCC_PLLCFGR_PLLN_Msk)
                | RCC_PLLCFGR_PLLPEN  * 1
                | RCC_PLLCFGR_PLLP    * 0
                | RCC_PLLCFGR_PLLQEN  * 1
                | RCC_PLLQ_DIV
                | RCC_PLLCFGR_PLLREN  * 1
                | RCC_PLLR_DIV
                | RCC_PLLP_DIV;
  RCC->CR = RCC_CR_HSION        * 1   // 
            | RCC_CR_HSIKERON   * 0   // 
            | RCC_CR_HSEON      * 1   // 
            | RCC_CR_HSEBYP     * 0   // 
            | RCC_CR_CSSON      * 0   // 
            | RCC_CR_PLLON      * 1   // 
            | RCC_CR_DEFAULT_LSB;     // 
   /* Wait till PLL is ready */
  while (!(RCC->CR & RCC_CR_PLLRDY))
  {
  }

  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
    
  RCC->CFGR &=~ RCC_CFGR_HPRE;
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
   /* Wait till System clock is ready */
  while (!(RCC->CFGR & RCC_CFGR_SWS))
  {
  }

  /* Insure 1s transition state at intermediate medium speed clock based on DWT */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
  while (DWT->CYCCNT < 100);
  // Set AHB prescaler
  RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1     // HCLK = 160MHz
              | RCC_CFGR_PPRE1_DIV8   // APB1 prescaler PCLK1 = 20MHz
              | RCC_CFGR_PPRE2_DIV8;  // APB2 prescaler PCLK2 = 20MHz
  
  
  LL_SetSystemCoreClock (160000000);

  NVIC_SetPriorityGrouping (0x03U); // 16 pre-emption priority groups, 0 subpriority
  
  NVIC_SetPriority (RCC_IRQn, (configMAX_PRIORITIES - 1)); // Higher them max FreeRTOS interrupt
  NVIC_EnableIRQ (RCC_IRQn);
  
  // enable CSS
  RCC->CR = RCC_CR_HSION        * 1   // 
            | RCC_CR_HSIKERON   * 0   // 
            | RCC_CR_HSEON      * 1   // 
            | RCC_CR_HSEBYP     * 0   // 
            | RCC_CR_CSSON      * 1   // 
            | RCC_CR_PLLON      * 1   // 
            | RCC_CR_DEFAULT_LSB;     //
}


/*****************************************************************************//**
* \brief RCC HSE error - config HSI16 as PLL input
* \return None
*******************************************************************************/
void system_clock_reconfig (void)
{
#define RCC_PLLM_DIVI   RCC_PLLCFGR_PLLM_DIV_1    // HSI (16MHZ) devider 
#define RCC_PLLN_MULI   (uint32_t)(20U)                // PLL mul 16MHz * 20 = 320MHz
#define RCC_PLLQ_DIVI   RCC_PLLCFGR_PLLQ_DIV_4    // USB devider 360/4
#define RCC_PLLR_DIVI   RCC_PLLCFGR_PLLR_DIV_2    // SYSCLK devider 320MHz/2 = 160MHz
#define RCC_PLLP_DIVI   RCC_PLLCFGR_PLLP_DIV_16   // ADC_CLK devider  320MHz/16= 20MHz


  RCC->CR = RCC_CR_HSION        * 1   // 
            | RCC_CR_HSIKERON   * 0   // 
            | RCC_CR_HSEON      * 0   // 
            | RCC_CR_HSEBYP     * 0   // 
            | RCC_CR_CSSON      * 0   // 
            | RCC_CR_PLLON      * 0   // 
            | RCC_CR_DEFAULT_LSB;     //

   /* Wait till HSI is ready */
  while (!(RCC->CR & RCC_CR_HSIRDY))
  {
  }

  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI
                | RCC_PLLM_DIVI
                | ((RCC_PLLN_MULI << RCC_PLLCFGR_PLLN_Pos) & RCC_PLLCFGR_PLLN_Msk)
                | RCC_PLLCFGR_PLLPEN  * 1
                | RCC_PLLCFGR_PLLP    * 0
                | RCC_PLLCFGR_PLLQEN  * 1
                | RCC_PLLQ_DIVI
                | RCC_PLLCFGR_PLLREN  * 1
                | RCC_PLLR_DIVI
                | RCC_PLLP_DIVI;
  RCC->CR = RCC_CR_HSION        * 1   // 
            | RCC_CR_HSIKERON   * 0   // 
            | RCC_CR_HSEON      * 0   // 
            | RCC_CR_HSEBYP     * 0   // 
            | RCC_CR_CSSON      * 0   // 
            | RCC_CR_PLLON      * 1   // 
            | RCC_CR_DEFAULT_LSB;     //
   /* Wait till PLL is ready */
  while (!(RCC->CR & RCC_CR_PLLRDY))
  {
  }

  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
    
  RCC->CFGR &=~ RCC_CFGR_HPRE;
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
   /* Wait till System clock is ready */
  while (!(RCC->CFGR & RCC_CFGR_SWS))
  {
  }

  /* Insure 1s transition state at intermediate medium speed clock based on DWT */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
  while (DWT->CYCCNT < 100)
  {
  }
  // Set AHB prescaler
  RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1     // HCLK = 160MHz
              | RCC_CFGR_PPRE1_DIV8   // PCLK1 = 20MHz
              | RCC_CFGR_PPRE2_DIV8;  // PCLK2 = 20MHz
  
#ifdef _RELEASE_
  // on LSI gen 32kHz
  RCC->CSR |= RCC_CSR_LSION;
  while (!(RCC->CSR & RCC_CSR_LSIRDY))
  {
  }
#endif  // #ifdef _RELEASE_
  
  NVIC_SetPriority (RCC_IRQn, (configMAX_PRIORITIES - 1)); // Higher them max FreeRTOS interrupt
  NVIC_EnableIRQ (RCC_IRQn);
}


/*****************************************************************************//**
* \brief Global settings periferal clocks
* \return None
*******************************************************************************/
void periferals_clocks_enable (void)
{
  // Select clock source
  RCC->CCIPR =  RCC_CCIPR_USART1SEL_PCLK2   // USART1
              | RCC_CCIPR_USART2SEL_PCLK1   // USART2
              | RCC_CCIPR_USART3SEL_PCLK1   // USART3
              | RCC_CCIPR_UART4SEL_PCLK1    // UART4
              | RCC_CCIPR_UART5SEL_PCLK1    // UART5
              | RCC_CCIPR_LPUART1SEL_PCLK1  // LPUART1
              | RCC_CCIPR_I2C1SEL_PCLK1     // I2C1
              | RCC_CCIPR_I2C2SEL_PCLK1     // I2C2
              | RCC_CCIPR_I2C3SEL_PCLK1     // I2C3
              | RCC_CCIPR_LPTIM1SEL_PCLK1   // LPTIM1
              | RCC_CCIPR_SAI1SEL_SYSCLK    // SAI1
              | RCC_CCIPR_I2S23SEL_SYSCLK   // I2S2 I2S3
              | RCC_CCIPR_FDCANSEL_PCLK1    // FDCAN
              | RCC_CCIPR_CLK48SEL_HSI48    // CLK48, USB RNG
              | RCC_CCIPR_ADC12SEL_SYSCLK   // ADC1 ADC2
              | RCC_CCIPR_ADC345SEL_SYSCLK; // ADC3 ADC4 ADC5
  
  RCC->AHB1ENR = RCC_AHB1ENR_DMA1EN           * 1
                 | RCC_AHB1ENR_DMA2EN         * 0
                 | RCC_AHB1ENR_DMAMUX1EN      * 1
                 | RCC_AHB1ENR_CORDICEN       * 1
                 | RCC_AHB1ENR_FMACEN         * 0 
                 | RCC_AHB1ENR_FLASHEN        * 1  
                 | RCC_AHB1ENR_CRCEN          * 1;
  (void)RCC->AHB1ENR;  // read
  
  RCC->AHB2ENR = RCC_AHB2ENR_GPIOAEN          * 1 
                 | RCC_AHB2ENR_GPIOBEN        * 1    
                 | RCC_AHB2ENR_GPIOCEN        * 1    
                 | RCC_AHB2ENR_GPIODEN        * 0    
                 | RCC_AHB2ENR_GPIOEEN        * 0    
                 | RCC_AHB2ENR_GPIOFEN        * 1    
                 | RCC_AHB2ENR_GPIOGEN        * 0    
                 | RCC_AHB2ENR_ADC12EN        * 0    
                 | RCC_AHB2ENR_ADC345EN       * 0    
                 | RCC_AHB2ENR_DAC1EN         * 1      
                 | RCC_AHB2ENR_DAC3EN         * 0        
                 | RCC_AHB2ENR_RNGEN          * 1;
  (void)RCC->AHB2ENR;  // read
  
  RCC->AHB3ENR = RCC_AHB3ENR_QSPIEN           * 0;
  (void)RCC->AHB3ENR;  // read
  
  RCC->APB1ENR1 = RCC_APB1ENR1_TIM2EN         * 0   
                  | RCC_APB1ENR1_TIM3EN       * 0      
                  | RCC_APB1ENR1_TIM4EN       * 0      
                  | RCC_APB1ENR1_TIM6EN       * 0      
                  | RCC_APB1ENR1_TIM7EN       * 1      
                  | RCC_APB1ENR1_CRSEN        * 0      
                  | RCC_APB1ENR1_RTCAPBEN     * 0      
                  | RCC_APB1ENR1_WWDGEN       * 0      
                  | RCC_APB1ENR1_SPI2EN       * 0      
                  | RCC_APB1ENR1_SPI3EN       * 0      
                  | RCC_APB1ENR1_USART2EN     * 0      
                  | RCC_APB1ENR1_USART3EN     * 0      
                  | RCC_APB1ENR1_UART4EN      * 0      
                  | RCC_APB1ENR1_UART5EN      * 0      
                  | RCC_APB1ENR1_I2C1EN       * 0      
                  | RCC_APB1ENR1_I2C2EN       * 0      
                  | RCC_APB1ENR1_USBEN        * 0      
                  | RCC_APB1ENR1_FDCANEN      * 0      
                  | RCC_APB1ENR1_PWREN        * 1      
                  | RCC_APB1ENR1_I2C3EN       * 0      
                  | RCC_APB1ENR1_LPTIM1EN     * 0;
  (void)RCC->APB1ENR1;  // read
  
  RCC->APB1ENR2 = RCC_APB1ENR2_LPUART1EN      * 0
                  | RCC_APB1ENR2_UCPD1EN      * 0;
  (void)RCC->APB1ENR2;  // read
  
  RCC->APB2ENR = RCC_APB2ENR_SYSCFGEN         * 1     
                 | RCC_APB2ENR_TIM1EN         * 0        
                 | RCC_APB2ENR_SPI1EN         * 0        
                 | RCC_APB2ENR_TIM8EN         * 0        
                 | RCC_APB2ENR_USART1EN       * 1        
                 | RCC_APB2ENR_TIM15EN        * 1        
                 | RCC_APB2ENR_TIM16EN        * 0        
                 | RCC_APB2ENR_TIM17EN        * 0        
                 | RCC_APB2ENR_TIM20EN        * 0        
                 | RCC_APB2ENR_SAI1EN         * 0;
  (void)RCC->APB2ENR;  // read
}

/*****************************************************************************//**
* \brief RCC error interrupt
*******************************************************************************/
void RCC_IRQHandler (void)
{
  /* Check RCC CSSF flag */
  if (RCC->CIFR & RCC_CIFR_HSERDYF)
  {
    /* Clear RCC CSS pending bit */
    RCC->CICR = RCC_CICR_HSERDYC;
    // config normal HSE osc
    system_clock_config ();
  }
}



