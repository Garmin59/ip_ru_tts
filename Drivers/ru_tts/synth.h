/* synth.h -- Common speech synthesis related definitions and prototypes
 *
 * Copyright (C) 1990, 1991 Speech Research Laboratory, Minsk
 * Copyright (C) 2005 Igor Poretsky <poretsky@mlbox.ru>
 * Copyright (C) 2021 Boris Lobanov <lobbormef@gmail.com>
 * Copyright (C) 2021 Alexander Ivanov <ivalex01@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdlib.h>
#include "ru_tts_data.h"
#include "transcription.h"
#include "sink.h"
#include "timing.h"
#include "modulation.h"
#include "soundscript.h"

/* TTS control data structure */
typedef struct
{
  transcription_state_t transcription_state;
  sink_t                wave_consumer;
  int                   flags;

  /* Speechrate parameters */
  timing_t              timing;

  /* Pitch and intonation control */
  modulation_t          modulation;
  // а ышшо добавим сюда саундскрипт, чтоб тупо его потом отдать. Пока лениво особо соображать, как это сделать максимально красиво
  soundscript_t         script;
  ru_tts_callback       beforeMakeSound;
} ttscb_t;


/* Function prototypes */

/*
 * Transcription callback function.
 *
 * Finalizes provided transcription and performs speech synthesis for it.
 *
 * The buffer pointed by the first argument should contain a transcription
 * of length specified by the second argument. The last argument
 * should point to the corresponding ttscb structure.
 */
int synth_function (void *buffer, size_t length, void *user_data);

