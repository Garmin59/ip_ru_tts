/*******************************************************************************
* \file    gpio.c
* \author  Ilya Petrukhin
* \brief   This file provides code for the configuration
*          of all used GPIO pins.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include	"stm32g4xx.h"				    // определения регистров и битовых переменных
#include	"core_cm4.h"				    // определения инлайнов
#include  "gpio.h"


/*****************************************************************************//** 
* \brief Configure pins as
*         Analog
*         Input
*         Output
*         EVENT_OUT
*         EXTI
*******************************************************************************/
void all_GPIO_init (void)
{
/** PORT A */
  GPIOA->ODR = 0x1800;
	GPIOA->MODER = GPIO_MODER_MODER0_OUTPUT       // PinA0 - Not used
              | GPIO_MODER_MODER1_OUTPUT        // PinA1 - Not used
              | GPIO_MODER_MODER2_OUTPUT        // PinA2 - Not used
              | GPIO_MODER_MODER3_OUTPUT        // PinA3 - Not used
              | GPIO_MODER_MODER4_ANALOG        // PinA4 - DAC1 CH1 OUT
              | GPIO_MODER_MODER5_ANALOG        // PinA5 - DAC1 CH2 OUT // LED
              | GPIO_MODER_MODER6_OUTPUT        // PinA6 - Not used
              | GPIO_MODER_MODER7_OUTPUT        // PinA7 - Not used
              | GPIO_MODER_MODER8_OUTPUT        // PinA8 - Not used
              | GPIO_MODER_MODER9_OUTPUT        // PinA9 - Not used
              | GPIO_MODER_MODER10_OUTPUT       // PinA10 - Not used
              | GPIO_MODER_MODER11_OUTPUT       // PinA11 - Not used
              | GPIO_MODER_MODER12_OUTPUT       // PinA12 - Not used
              | GPIO_MODER_MODER13_ALTERNATIVE  // PinA13 - SWDIO
              | GPIO_MODER_MODER14_ALTERNATIVE  // PinA14 - SDCLK
              | GPIO_MODER_MODER15_ALTERNATIVE; // PinA15 - JTDI
  GPIOA->AFR[0] = GPIO_AFR_AFSEL0_AF0           // PinA0 - Not used
              | GPIO_AFR_AFSEL1_AF0             // PinA1 - Not used
              | GPIO_AFR_AFSEL2_AF0             // PinA2 - Not used
              | GPIO_AFR_AFSEL3_AF0             // PinA3 - Not used
              | GPIO_AFR_AFSEL4_AF0             // PinA4 - DAC1 CH1 OUT
              | GPIO_AFR_AFSEL5_AF0             // PinA5 - DAC1 CH2 OUT // LED
              | GPIO_AFR_AFSEL6_AF0             // PinA6 - Not used
              | GPIO_AFR_AFSEL7_AF0;            // PinA7 - Not used
  GPIOA->AFR[1] = GPIO_AFR_AFSEL8_AF0           // PinA8 - Not used
              | GPIO_AFR_AFSEL9_AF0             // PinA9 - Not used
              | GPIO_AFR_AFSEL10_AF0            // PinA10 - Not used
              | GPIO_AFR_AFSEL11_AF0            // PinA11 - Not used
              | GPIO_AFR_AFSEL12_AF0            // PinA12 - Not used
              | GPIO_AFR_AFSEL13_AF0            // PinA13 - SWDIO
              | GPIO_AFR_AFSEL14_AF0            // PinA14 - SDCLK
              | GPIO_AFR_AFSEL15_AF0;           // PinA15 - JTDI
  GPIOA->OTYPER = GPIO_OTYPER_OT0_PP			      // PinA0 - Not used
              | GPIO_OTYPER_OT1_PP			        // PinA1 - Not used
              | GPIO_OTYPER_OT2_PP			        // PinA2 - Not used
              | GPIO_OTYPER_OT3_PP			        // PinA3 - Not used
              | GPIO_OTYPER_OT4_PP			        // PinA4 - Not used
              | GPIO_OTYPER_OT5_PP			        // PinA5 - LED
              | GPIO_OTYPER_OT6_PP			        // PinA6 - Not used
              | GPIO_OTYPER_OT7_PP			        // PinA7 - Not used
              | GPIO_OTYPER_OT8_PP			        // PinA8 - Not used
              | GPIO_OTYPER_OT9_PP			        // PinA9 - Not used
              | GPIO_OTYPER_OT10_PP			        // PinA10 - Not used
              | GPIO_OTYPER_OT11_PP			        // PinA11 - Not used
              | GPIO_OTYPER_OT12_PP			        // PinA12 - Not used
              | GPIO_OTYPER_OT13_PP			        // PinA13 - SWDIO
              | GPIO_OTYPER_OT14_PP			        // PinA14 - SDCLK
              | GPIO_OTYPER_OT15_PP;			      // PinA15 - JTDI
	GPIOA->OSPEEDR = GPIO_OSPEEDR_OSPEEDR0_LOW    // PinA0 - Not used
              | GPIO_OSPEEDR_OSPEEDR1_LOW       // PinA1 - Not used
              | GPIO_OSPEEDR_OSPEEDR2_LOW       // PinA2 - Not used
              | GPIO_OSPEEDR_OSPEEDR3_LOW       // PinA3 - Not used
              | GPIO_OSPEEDR_OSPEEDR4_LOW       // PinA4 - Not used
              | GPIO_OSPEEDR_OSPEEDR5_LOW       // PinA5 - LED
              | GPIO_OSPEEDR_OSPEEDR6_LOW       // PinA6 - Not used
              | GPIO_OSPEEDR_OSPEEDR7_LOW       // PinA7 - Not used
              | GPIO_OSPEEDR_OSPEEDR8_MED       // PinA8 - Not used
              | GPIO_OSPEEDR_OSPEEDR9_LOW       // PinA9 - Not used
              | GPIO_OSPEEDR_OSPEEDR10_LOW      // PinA10 - Not used
              | GPIO_OSPEEDR_OSPEEDR11_LOW      // PinA11 - Not used
              | GPIO_OSPEEDR_OSPEEDR12_LOW      // PinA12 - Not used
              | GPIO_OSPEEDR_OSPEEDR13_HIGH     // PinA13 - SWDIO
              | GPIO_OSPEEDR_OSPEEDR14_HIGH     // PinA14 - SDCLK
              | GPIO_OSPEEDR_OSPEEDR15_HIGH;    // PinA15 - JTDI
	GPIOA->PUPDR = GPIO_PUPDR_PUPDR0_NO		        // PinA0 без подтяжки
              | GPIO_PUPDR_PUPDR1_NO		        // PinA1 без подтяжки
              | GPIO_PUPDR_PUPDR2_NO		        // PinA2 без подтяжки
              | GPIO_PUPDR_PUPDR3_NO		        // PinA3 без подтяжки
              | GPIO_PUPDR_PUPDR4_NO  		      // PinA4 без подтяжки
              | GPIO_PUPDR_PUPDR5_NO  		      // PinA5 без подтяжки
              | GPIO_PUPDR_PUPDR6_NO  		      // PinA6 без подтяжки
              | GPIO_PUPDR_PUPDR7_NO  		      // PinA7 без подтяжки
              | GPIO_PUPDR_PUPDR8_NO		        // PinA8 без подтяжки
              | GPIO_PUPDR_PUPDR9_NO		        // PinA9 без подтяжки
              | GPIO_PUPDR_PUPDR10_NO		        // PinA10 без подтяжки
              | GPIO_PUPDR_PUPDR11_NO		        // PinA11 без подтяжки
              | GPIO_PUPDR_PUPDR12_NO		        // PinA12 без подтяжки
              | GPIO_PUPDR_PUPDR13_NO		        // PinA13 без подтяжки
              | GPIO_PUPDR_PUPDR14_NO		        // PinA14 без подтяжки
              | GPIO_PUPDR_PUPDR15_NO;		      // PinA15 без подтяжки


/** Port B */
  GPIOB->ODR = 0x1806;
	GPIOB->MODER = GPIO_MODER_MODER0_OUTPUT       // PinB0 - Not used
              | GPIO_MODER_MODER1_OUTPUT        // PinB1 - Not used
              | GPIO_MODER_MODER2_OUTPUT        // PinB2 - Not used
              | GPIO_MODER_MODER3_ALTERNATIVE   // PinB3 - SWO 
              | GPIO_MODER_MODER4_OUTPUT        // PinB4 - Not used
              | GPIO_MODER_MODER5_OUTPUT        // PinB5 - Not used
              | GPIO_MODER_MODER6_OUTPUT        // PinB6 - Not used
              | GPIO_MODER_MODER7_OUTPUT        // PinB7 - Not used
              | GPIO_MODER_MODER8_INPUT         // PinB8 - BOOT
              | GPIO_MODER_MODER9_OUTPUT        // PinB9 - Not used
              | GPIO_MODER_MODER10_OUTPUT       // PinB10 - Not used
              | GPIO_MODER_MODER11_OUTPUT       // PinB11 - Not used
              | GPIO_MODER_MODER12_OUTPUT       // PinB12 - Not used
              | GPIO_MODER_MODER13_OUTPUT       // PinB13 - Not used
              | GPIO_MODER_MODER14_OUTPUT       // PinB14 - Not used
              | GPIO_MODER_MODER15_OUTPUT;      // PinB15 - Not used
  GPIOB->AFR[0] = GPIO_AFR_AFSEL0_AF0           // PinB0 - Not used
              | GPIO_AFR_AFSEL1_AF0             // PinB1 - Not used
              | GPIO_AFR_AFSEL2_AF0             // PinB2 - Not used
              | GPIO_AFR_AFSEL3_AF0             // PinB3 - SWO 
              | GPIO_AFR_AFSEL4_AF0             // PinB4 - Not used
              | GPIO_AFR_AFSEL5_AF0             // PinB5 - Not used
              | GPIO_AFR_AFSEL6_AF0             // PinB6 - Not used
              | GPIO_AFR_AFSEL7_AF0;            // PinB7 - Not used
  GPIOB->AFR[1] =  GPIO_AFR_AFSEL8_AF0          // PinB8 - BOOT
              | GPIO_AFR_AFSEL9_AF0             // PinB9 - Not used
              | GPIO_AFR_AFSEL10_AF0            // PinB10 - Not used
              | GPIO_AFR_AFSEL11_AF0            // PinB11 - Not used
              | GPIO_AFR_AFSEL12_AF0            // PinB12 - Not used
              | GPIO_AFR_AFSEL13_AF0            // PinB13 - Not used
              | GPIO_AFR_AFSEL14_AF0            // PinB14 - Not used
              | GPIO_AFR_AFSEL15_AF0;           // PinB15 - Not used
  GPIOB->OTYPER = GPIO_OTYPER_OT0_PP			      // PinB0 - Not used
              | GPIO_OTYPER_OT1_PP			        // PinB1 - Not used
              | GPIO_OTYPER_OT2_PP			        // PinB2 - Not used
              | GPIO_OTYPER_OT3_PP			        // PinB3 - SWO 
              | GPIO_OTYPER_OT4_PP			        // PinB4 - Not used
              | GPIO_OTYPER_OT5_PP			        // PinB5 - Not used
              | GPIO_OTYPER_OT6_PP			        // PinB6 - Not used
              | GPIO_OTYPER_OT7_PP			        // PinB7 - Not used
              | GPIO_OTYPER_OT8_PP			        // PinB8 - BOOT
              | GPIO_OTYPER_OT9_PP              // PinB9 - Not used
              | GPIO_OTYPER_OT10_PP             // PinB10 - Not used
              | GPIO_OTYPER_OT11_PP             // PinB11 - Not used
              | GPIO_OTYPER_OT12_PP             // PinB12 - Not used
              | GPIO_OTYPER_OT13_PP             // PinB13 - Not used
              | GPIO_OTYPER_OT14_PP             // PinB14 - Not used
              | GPIO_OTYPER_OT15_PP;            // PinB15 - Not used
	GPIOB->OSPEEDR = GPIO_OSPEEDR_OSPEEDR0_LOW    // PinB0 - Not used
              | GPIO_OSPEEDR_OSPEEDR1_LOW       // PinB1 - Not used
              | GPIO_OSPEEDR_OSPEEDR2_LOW       // PinB2 - Not used
              | GPIO_OSPEEDR_OSPEEDR3_LOW       // PinB3 - Not used 
              | GPIO_OSPEEDR_OSPEEDR4_MED       // PinB4 - Not used
              | GPIO_OSPEEDR_OSPEEDR5_MED       // PinB5 - Not used
              | GPIO_OSPEEDR_OSPEEDR6_MED       // PinB6 - Not used
              | GPIO_OSPEEDR_OSPEEDR7_LOW       // PinB7 - Not used
              | GPIO_OSPEEDR_OSPEEDR8_LOW       // PinB8 - BOOT
              | GPIO_OSPEEDR_OSPEEDR9_LOW       // PinB9 - Not used
              | GPIO_OSPEEDR_OSPEEDR10_LOW      // PinB10 - Not used
              | GPIO_OSPEEDR_OSPEEDR11_LOW      // PinB11 - Not used
              | GPIO_OSPEEDR_OSPEEDR12_LOW      // PinB12 - Not used
              | GPIO_OSPEEDR_OSPEEDR13_LOW      // PinB13 - Not used
              | GPIO_OSPEEDR_OSPEEDR14_LOW      // PinB14 - Not used
              | GPIO_OSPEEDR_OSPEEDR15_LOW;     // PinB15 - Not used
 	GPIOB->PUPDR = GPIO_PUPDR_PUPDR0_NO		        // PinB0 без подтяжки
              | GPIO_PUPDR_PUPDR1_NO		        // PinB1 без подтяжки
              | GPIO_PUPDR_PUPDR2_NO		        // PinB2 без подтяжки
              | GPIO_PUPDR_PUPDR3_NO		        // PinB3 без подтяжки
              | GPIO_PUPDR_PUPDR4_NO  		      // PinB4 без подтяжки
              | GPIO_PUPDR_PUPDR5_NO  		      // PinB5 без подтяжки
              | GPIO_PUPDR_PUPDR6_NO  		      // PinB6 без подтяжки
              | GPIO_PUPDR_PUPDR7_NO  		      // PinB7 без подтяжки
              | GPIO_PUPDR_PUPDR8_NO  		      // PinB8 без подтяжки
              | GPIO_PUPDR_PUPDR9_NO  		      // PinB9 без подтяжки
              | GPIO_PUPDR_PUPDR10_NO  		      // PinB10 без подтяжки
              | GPIO_PUPDR_PUPDR11_NO  		      // PinB11 без подтяжки
              | GPIO_PUPDR_PUPDR12_NO  		      // PinB12 без подтяжки
              | GPIO_PUPDR_PUPDR13_NO  		      // PinB13 без подтяжки
              | GPIO_PUPDR_PUPDR14_NO  		      // PinB14 без подтяжки
              | GPIO_PUPDR_PUPDR15_NO; 		      // PinB15 без подтяжки


/** Port C */
  GPIOC->ODR = 0x0000;
	GPIOC->MODER = GPIO_MODER_MODER0_OUTPUT       // PinC0 - 
              | GPIO_MODER_MODER1_OUTPUT        // PinC1 - 
              | GPIO_MODER_MODER2_OUTPUT        // PinC2 - 
              | GPIO_MODER_MODER3_OUTPUT        // PinC3 - 
              | GPIO_MODER_MODER4_ALTERNATIVE   // PinC4 - USART1_TX
              | GPIO_MODER_MODER5_ALTERNATIVE   // PinC5 - USART1_RX
              | GPIO_MODER_MODER6_OUTPUT        // PinC6 - 
              | GPIO_MODER_MODER7_OUTPUT        // PinC7 - 
              | GPIO_MODER_MODER8_OUTPUT        // PinC8 - 
              | GPIO_MODER_MODER9_OUTPUT        // PinC9 - test pin
              | GPIO_MODER_MODER10_OUTPUT       // PinC10 -
              | GPIO_MODER_MODER11_OUTPUT       // PinC11 - 
              | GPIO_MODER_MODER12_OUTPUT       // PinC12 - 
              | GPIO_MODER_MODER13_INPUT        // PinC13 - BUTTON
              | GPIO_MODER_MODER14_OUTPUT       // PinC14 -
              | GPIO_MODER_MODER15_OUTPUT;      // PinC15 -
  GPIOC->AFR[0] = GPIO_AFR_AFSEL0_AF0           // PinC0 - 
              | GPIO_AFR_AFSEL1_AF0             // PinC1 - 
              | GPIO_AFR_AFSEL2_AF0             // PinC2 - 
              | GPIO_AFR_AFSEL3_AF0             // PinC3 - 
              | GPIO_AFR_AFSEL4_AF7             // PinC4 - USART1_TX
              | GPIO_AFR_AFSEL5_AF7             // PinC5 - USART1_RX
              | GPIO_AFR_AFSEL6_AF0             // PinC6 - 
              | GPIO_AFR_AFSEL7_AF0;            // PinC7 - 
  GPIOC->AFR[1] =  GPIO_AFR_AFSEL8_AF0          // PinC8 - 
              | GPIO_AFR_AFSEL9_AF0             // PinC9 - test pin
              | GPIO_AFR_AFSEL10_AF0            // PinC10 -
              | GPIO_AFR_AFSEL11_AF0            // PinC11 - 
              | GPIO_AFR_AFSEL12_AF0            // PinC12 - 
              | GPIO_AFR_AFSEL13_AF0            // PinC13 - BUTTON
              | GPIO_AFR_AFSEL14_AF0            // PinC14 -
              | GPIO_AFR_AFSEL15_AF0;           // PinC15 -
  GPIOC->OTYPER = GPIO_OTYPER_OT0_PP			      // PinC0 - 
              | GPIO_OTYPER_OT1_PP			        // PinC1 - 
              | GPIO_OTYPER_OT2_PP			        // PinC2 - 
              | GPIO_OTYPER_OT3_PP			        // PinC3 - 
              | GPIO_OTYPER_OT4_PP			        // PinC4 - USART1_TX
              | GPIO_OTYPER_OT5_PP			        // PinC5 - USART1_RX
              | GPIO_OTYPER_OT6_PP			        // PinC6 - 
              | GPIO_OTYPER_OT7_PP			        // PinC7 - 
              | GPIO_OTYPER_OT8_PP			        // PinC8 - 
              | GPIO_OTYPER_OT9_PP              // PinC9 - test pin
              | GPIO_OTYPER_OT10_PP             // PinC10 -
              | GPIO_OTYPER_OT11_PP             // PinC11 - 
              | GPIO_OTYPER_OT12_PP             // PinC12 - 
              | GPIO_OTYPER_OT13_PP             // PinC13 - BUTTON
              | GPIO_OTYPER_OT14_PP             // PinC14 -
              | GPIO_OTYPER_OT15_PP;            // PinC15 -
	GPIOC->OSPEEDR = GPIO_OSPEEDR_OSPEEDR0_LOW    // PinC0 - 
              | GPIO_OSPEEDR_OSPEEDR1_LOW       // PinC1 - 
              | GPIO_OSPEEDR_OSPEEDR2_LOW       // PinC2 - 
              | GPIO_OSPEEDR_OSPEEDR3_LOW       // PinC3 - 
              | GPIO_OSPEEDR_OSPEEDR4_HIGH      // PinC4 - USART1_TX
              | GPIO_OSPEEDR_OSPEEDR5_HIGH      // PinC5 - USART1_RX
              | GPIO_OSPEEDR_OSPEEDR6_LOW       // PinC6 - 
              | GPIO_OSPEEDR_OSPEEDR7_LOW       // PinC7 - 
              | GPIO_OSPEEDR_OSPEEDR8_LOW       // PinC8 - 
              | GPIO_OSPEEDR_OSPEEDR9_LOW       // PinC9 -
              | GPIO_OSPEEDR_OSPEEDR10_LOW      // PinC10 -
              | GPIO_OSPEEDR_OSPEEDR11_LOW      // PinC11 - 
              | GPIO_OSPEEDR_OSPEEDR12_LOW      // PinC12 - 
              | GPIO_OSPEEDR_OSPEEDR13_LOW      // PinC13 - 
              | GPIO_OSPEEDR_OSPEEDR14_LOW      // PinC14 -
              | GPIO_OSPEEDR_OSPEEDR15_LOW;     // PinC15 -
 	GPIOC->PUPDR = GPIO_PUPDR_PUPDR0_NO		        // PinC0 без подтяжки
              | GPIO_PUPDR_PUPDR1_NO		        // PinC1 без подтяжки
              | GPIO_PUPDR_PUPDR2_NO		        // PinC2 без подтяжки
              | GPIO_PUPDR_PUPDR3_NO		        // PinC3 без подтяжки
              | GPIO_PUPDR_PUPDR4_NO  		      // PinC4 без подтяжки
              | GPIO_PUPDR_PUPDR5_NO  		      // PinC5 без подтяжки
              | GPIO_PUPDR_PUPDR6_NO  		      // PinC6 без подтяжки
              | GPIO_PUPDR_PUPDR7_NO  		      // PinC7 без подтяжки
              | GPIO_PUPDR_PUPDR8_NO  		      // PinC8 без подтяжки
              | GPIO_PUPDR_PUPDR9_NO  		      // PinC9 без подтяжки
              | GPIO_PUPDR_PUPDR10_NO  		      // PinC10 без подтяжки
              | GPIO_PUPDR_PUPDR11_NO  		      // PinC11 без подтяжки
              | GPIO_PUPDR_PUPDR12_NO  		      // PinC12 без подтяжки
              | GPIO_PUPDR_PUPDR13_NO  		      // PinC13 без подтяжки
              | GPIO_PUPDR_PUPDR14_NO  		      // PinC14 без подтяжки
              | GPIO_PUPDR_PUPDR15_NO;  		    // PinC15 без подтяжки

/** Port F */
// OSC pin default

/** Port G */
// NRST pin default
}



