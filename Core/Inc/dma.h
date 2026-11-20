/**************************************************************
 * @file dma.h
 * @author Ilya Petrukhin (ilya.petrukhin@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-08-2023
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************/

#ifndef _DMA_H_
#define _DMA_H_

void *memcpy_dma (void *dest, void *src, uint16_t size);
void usart1_tx_dma_init (void *tx_buffer, uint16_t tx_size);
void usart1_rx_dma_init (void *rx_buffer, uint16_t rx_buffer_size);
void dma1_ch4_init (void);
void dma1_ch5_init (void);

void dma1_ch4_enable (void *buffer, uint16_t size);
void dma1_ch5_enable (void *buffer, uint16_t size);

#endif // _DMA_H_