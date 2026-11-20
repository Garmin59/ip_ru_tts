/**************************************************************
 * @file circular_buffer.h
 * @author Ilya Petrukhin (ilya.petrukhin@gmail.com)
 * @brief 
 * @version 0.1
 * @date 03-08-2023
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************/
#ifndef _CIRCULAR_BUFFER_
#define _CIRCULAR_BUFFER_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct _circular_buffer_t
{
  uint8_t             *buffer;
  uint32_t            buf_size;
  volatile uint32_t   *head;    
  volatile uint32_t   *tale;
} circular_buffer_t;

bool init_circular_buffer (circular_buffer_t *buff, uint8_t *databuff, uint32_t buf_size);
bool is_circular_buffer_have_space (circular_buffer_t *buff, uint32_t size);
bool put_circular_buffer (circular_buffer_t *buff, uint8_t *data, uint32_t size);
bool get_circular_buffer (circular_buffer_t *buff, uint8_t *data, uint32_t *size);
static inline bool is_circular_buffer_have_message (circular_buffer_t *buff);


/**************************************************************
 * @brief ask to have a message in the circular buffer
 * 
 * @param buff    *circular_buffer_t
 * @return true   buffer have a message
 * @return false  buffer don't have a message
**************************************************************/
static inline bool is_circular_buffer_have_message (circular_buffer_t *buff)
{
  return (buff->head != buff->tale);
}

#endif // _CIRCULAR_BUFFER_

