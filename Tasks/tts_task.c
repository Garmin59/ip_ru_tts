/******************************************************************************
 * @file tts_task.c
 * @author Ilya Petrukhin ()
 * @brief
 * @version 0.1
 * @date 19-07-2026
 *
 * @copyright Copyright (c) 2026
 *
 ******************************************************************************/

#include <iconv.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "global.h"
#include "tts_task.h"
#include "ru_tts.h"
#include "usart.h"

#define UTF_STR_MAX_LEN 1024
#define KOI_STR_MAX_LEN 1024

static ttscb_t ttscb; // определим её здесь, чтобы посмотреть немножко потрохов
static char wave[4096 * 2];

static ru_tts_conf_t config =
{
  .speech_rate = 100,              /* Reasonable value range is [20..500]. */
  .voice_pitch = 100,              /* Reasonable value range is [50..300]. */
  .intonation = 20,               /* Reasonable value range is [0..140].
                                  Greater values imply more expressive speech. */
  /* Interclause gap durations adjustment */
  .general_gap_factor = 0,       /* Reasonable value range is [0..x].
                                  Upper boundary depends on speech rate.
                                  At the minimum speech rate it is 1.25.
                                  Quicker speech allows to use greater values. */
  .comma_gap_factor = 100,         /* Reasonable value range is [0..750] */
  .dot_gap_factor = 100,           /* Reasonable value range is [0..500] */
  .semicolon_gap_factor = 100,     /* Reasonable value range is [0..600] */
  .colon_gap_factor = 100,         /* Reasonable value range is [0..600] */
  .question_gap_factor = 100,      /* Reasonable value range is [0..375] */
  .exclamation_gap_factor = 100,   /* Reasonable value range is [0..300] */
  .intonational_gap_factor = 100,  /* Reasonable value range is [0..1000] */

  /* Combination of TTS control flags */
  .flags = DEC_SEP_POINT,
};


int wave_output (void *buffer, size_t length, void *ttscb)
{
  bool ret = put_circular_buffer (&g.tts_out_buffer, buffer, length);
  static uint8_t data = 0;
  if (ret)
  {
    xQueueSend (g.tts_out_queue, &data, portMAX_DELAY);
    data++;
  }
  return ret;
}


/******************************************************************************
 * @brief ru tts task
 *
 * @param arg
 ******************************************************************************/
void tts_task(void *arg)
{
  static queue_data tts_data;
  // static char buff[KOI_STR_MAX_LEN];
  // static char *str = buff;
  
  int write_error;

  ru_tts_config_init (&config);


  while (1)
  {
    if (xQueueReceive (g.tts_queue, &tts_data, portMAX_DELAY))
    {
      size_t len = strlen(tts_data.buffer);
      (void)len;
      ru_tts_transfer (&config, &ttscb, tts_data.buffer, wave, sizeof(wave), wave_output, &write_error);
    }
  }
}

