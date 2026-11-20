/**************************************************************
 * @file circular_buffer.c
 * @author Ilya Petrukhin (ilya.petrukhin@gmail.com)
 * @brief 
 * @version 0.1
 * @date 03-08-2023
 * 
 * @copyright Copyright (c) 2023
 * 
**************************************************************/

#include "circular_buffer.h"
// #include "dma.h"
#include <string.h>
#define memcpy_dma(a,b,c) memcpy(a,b,c)

/**************************************************************
 * @brief init struct circular_buffer_t
 * 
 * @param buff      pointer to circular_buffer_t struct
 * @param databuff  buffer to storage data
 * @param buf_size  size storage buffer
 * @return true     success init
 * @return false    buff size not aligned to 4 bytes
**************************************************************/
bool init_circular_buffer (circular_buffer_t *buff, uint8_t *databuff, uint32_t buf_size)
{
  if (buff->buf_size & 0x3)
  {
    return false;
  }
  buff->buffer = databuff;
  buff->buf_size = buf_size;
  buff->tale = buff->head = (uint32_t *)databuff;
  return true;
}


/**************************************************************
 * @brief calculate to have space for storage data
 * 
 * @param buff    * circular_buffer_t
 * @param size    size to storage in bytes
 * @return true   have space to put data
 * @return false  no have space
**************************************************************/
bool is_circular_buffer_have_space (circular_buffer_t *buff, uint32_t size)
{
  uint32_t align_size = (size & 0x3) ? ((size & ~(uint32_t)0x3) + 0x4) : size;
  align_size += sizeof(uint32_t *); // for the next head pointer

  if (buff->head >= buff->tale) // buffer in normal state
  {
    return (((uint32_t)buff->tale + buff->buf_size - (uint32_t)buff->head) > align_size);
  }
  // head < tale, buffer in reverse state
  return (((uint32_t)buff->tale - (uint32_t)buff->head) > align_size);
}

/**************************************************************
 * @brief put the message into circular buffer
 * 
 * @param buff    * circular_buffer_t
 * @param data    pointer to the message
 * @param size    message size
 * @return true   ok
 * @return false  not have a space in the circular buffer
**************************************************************/
bool put_circular_buffer (circular_buffer_t *buff, uint8_t *data, uint32_t size)
{
  uint32_t high_part;

  if (is_circular_buffer_have_space (buff, size))
  {
    *buff->head = size;
    uint32_t align_size = (size & 0x3) ? ((size & ~(uint32_t)0x3) + 0x4) : size;
    uint8_t *start = (uint8_t *)((uint32_t)buff->head + sizeof (uint32_t *));
    uint8_t *buff_end = (uint8_t *)((uint32_t)buff->buffer + buff->buf_size);
    
    if (buff->head >= buff->tale) // head >= tale, normal state
    {
      if (start >= buff_end)      // start from bottom, one part
      {
        buff->head = (uint32_t *)((uint32_t)buff->buffer + align_size);
        memcpy_dma ((void *)buff->buffer, data, size);
      }
      else                        // start from head
      {
        high_part = (uint32_t)(buff_end - start); //     

        if (high_part >= size)    // one part
        {
          buff->head = (uint32_t *)((uint32_t)start + align_size); // one part, continue high
          if ((uint32_t)buff->head >= (uint32_t)buff_end)     // one part, start next from bottom
          {
            buff->head = (uint32_t *)buff->buffer;
          }
          memcpy_dma (start, data, size);
        }
        else  // (high_part < align_size) two parts
        {
          uint32_t low_part = size - high_part;
          buff->head = (uint32_t *)((uint32_t)buff->buffer + align_size - high_part);
          memcpy_dma (start, data, high_part);
          memcpy_dma ((void *)buff->buffer, &data[high_part], low_part);
        }
      }
    }
    else  // head < tale, reverse state
    {
      buff->head = (uint32_t *)((uint32_t)start + align_size);
      memcpy_dma (start, data, size);
    }
    return true;
  }
  // not have a free space
  return false;
}

/**************************************************************
 * @brief Get the circular buffer object
 * 
 * @param buff  pointer to the circular_buffer_t
 * @param data  buffer to receive message
 * @param psize pointer to receive data
 * @return true   ok
 * @return false  no messages
**************************************************************/
bool get_circular_buffer (circular_buffer_t *buff, uint8_t *data, uint32_t *psize)
{
  if (is_circular_buffer_have_message (buff))
  {
    *psize = *buff->tale;         // put size
    uint32_t size = *buff->tale;
    uint32_t align_size = (size & 0x3) ? ((size & ~(uint32_t)0x3) + 0x4) : size;
    uint8_t *start = (uint8_t *)((uint32_t)buff->tale + sizeof (uint32_t *));

    if (buff->head > buff->tale)  // head > tale, normal state
    {
      buff->tale = (uint32_t *)((uint32_t)start + align_size);  // 
      memcpy_dma (data, start, size);
    }
    else  // tale > head, reverse state
    {
      uint8_t *buff_end = (uint8_t *)((uint32_t)buff->buffer + buff->buf_size);

      if (start >= buff_end)      // reverse state, one part from bottom
      {
        buff->tale = (uint32_t *)((uint32_t)buff->buffer + align_size);
        memcpy_dma (data, (void *)buff->buffer, size);
      }
      else                        // start < buff_end reverse state
      {
        uint32_t high_part = (uint32_t)(buff_end - start); //

        if (high_part >= size)    // one part up to top
        {
          buff->tale = (uint32_t *)((uint32_t)start + align_size);  // 
          if ((uint32_t)buff->tale >= (uint32_t)buff_end)   // tale from the bottom
          {
            buff->tale = (uint32_t *)buff->buffer;
          }
          memcpy_dma (data, start, size);
        }
        else                  // two parts
        {
          uint32_t low_part = size - high_part;
          buff->tale = (uint32_t *)((uint32_t)buff->buffer + align_size - high_part);
          memcpy_dma (data, start, high_part);
          memcpy_dma (&data[high_part], (void *)buff->buffer, low_part);
        }
      }
    }
    return true;
  }
  // not have a message
  return false;
}

