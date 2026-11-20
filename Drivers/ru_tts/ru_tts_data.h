/******************************************************************************
 * @file ru_tts_data.h
 * @author Ilya Petrukhin ()
 * @brief 
 * @version 0.1
 * @date 19-07-2026
 * 
 * @copyright Copyright (c) 2026
 * 
******************************************************************************/
#pragma once

#include <strings.h>

// typedefs ru tts datatypes

/* Callback function to utilize generated sound */
typedef int (*ru_tts_callback)(void *buffer, size_t size, void *user_data);

/* Speech parameters */
typedef struct
{
  int speech_rate;              /* Reasonable value range is [20..500]. */
  int voice_pitch;              /* Reasonable value range is [50..300]. */
  int intonation;               /* Reasonable value range is [0..140].
                                  Greater values imply more expressive speech. */

  /* Interclause gap durations adjustment */
  int general_gap_factor;       /* Reasonable value range is [0..x].
                                  Upper boundary depends on speech rate.
                                  At the minimum speech rate it is 1.25.
                                  Quicker speech allows to use greater values. */
  int comma_gap_factor;         /* Reasonable value range is [0..750] */
  int dot_gap_factor;           /* Reasonable value range is [0..500] */
  int semicolon_gap_factor;     /* Reasonable value range is [0..600] */
  int colon_gap_factor;         /* Reasonable value range is [0..600] */
  int question_gap_factor;      /* Reasonable value range is [0..375] */
  int exclamation_gap_factor;   /* Reasonable value range is [0..300] */
  int intonational_gap_factor;  /* Reasonable value range is [0..1000] */

  /* Combination of TTS control flags */
  int flags;
} ru_tts_conf_t;

