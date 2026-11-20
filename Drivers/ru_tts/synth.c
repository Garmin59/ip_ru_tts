/* synth.c -- Phonetic transcription to speech transformation
 *
 * Copyright (C) 1990, 1991 Speech Research Laboratory, Minsk
 * Copyright (C) 2005 Igor Poretsky <poretsky@mlbox.ru>
 * Copyright (C) 2021 Boris Lobanov <lobbormef@gmail.com>
 * Copyright (C) 2021 Alexander Ivanov <ivalex01@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "synth.h"
#include "soundscript.h"
#include "transcription.h"
#include "ru_tts.h"

/* Local data */
// традиционно: сначала длина в байтах, затем последовательность.
// чтобы перейти на последовательность под номером N, надо N раз прыгать начиная с нулевого байта.
static const uint8_t seqlist1[] =
    {
        1, 2,                // А
        5, 6, 8, 1, 24, 3,   // ВРОД'Э
        4, 21, 2, 25, 3,     // ДАГ'Э (видимо, "даже", но оно так не транскрибируется, выходит ДАЖЭ)
        2, 21, 2,            // ДА
        5, 10, 3, 21, 6, 2,  // ЙЭДВА
        5, 10, 3, 35, 17, 5, // ЙЭСЛ'И
        3, 5, 17, 5,         // ИЛ'И
        4, 5, 27, 2, 28,     // ИТАК
        5, 5, 16, 2, 33, 3,  // ИНАЧЭ
        1, 5,                // И
        3, 28, 2, 28,        // КАК
        4, 17, 5, 20, 1,     // Л'ИБО
        2, 16, 1,            // НО
        3, 8, 2, 35,         // РАС
        5, 27, 2, 28, 25, 3, // ТАКГ'Э (видимо ТАКЖЕ, но при транскрипции будет ТАКЖЭ)
        3, 27, 2, 28,        // ТАК
        3, 30, 3, 15,        // Т'ЭМ
        4, 27, 1, 25, 3,     // ТОГ'Э (видимо ТОЖЕ)
        3, 40, 1, 30,        // ХОТ'
        3, 33, 3, 15,        // ЧЭМ
        5, 33, 27, 1, 20, 4, // ЧТОБЫ
        4, 33, 27, 1, 26,    // ЧТОП (видимо чтоб, но при транскрипции нет правила заглушать последнюю согласную)
        3, 33, 27, 1,        // ЧТО
        4, 5, 27, 2, 22,     // ИТОГ
        3, 28, 2, 22,        // КАГ   (видимо КАК, но оно точно так не должно транскрибироваться)
        3, 8, 2, 7,          // РАЗ
        3, 27, 2, 22,        // ТАГ
        3, 40, 1, 24,        // ХОД'
        4, 33, 27, 1, 20,    // ЧТОБ
        2, 27, 1,            // ТО
        0};

static const uint8_t seqlist2[] =
    {
        3, 23, 3, 35,           // Б'ЭС
        3, 23, 3, 7,            // Б'ЭЗ
        4, 6, 21, 1, 17,        // ВДОЛ'
        4, 20, 17, 5, 35,       // БЛ'ИС
        4, 20, 17, 5, 7,        // БЛ'ИЗ
        5, 6, 8, 1, 24, 3,      // ВРОД'Э
        5, 6, 1, 7, 17, 3,      // ВОЗЛ'Э
        5, 6, 11, 5, 21, 0,     // ВВ'ИДУ
        3, 6, 19, 3,            // ВН'Э
        2, 6, 1,                // ВО
        1, 6,                   // В
        1, 34,                  // Ф
        3, 21, 17, 2,           // ДЛ'А
        2, 21, 1,               // ДО
        2, 5, 35,               // ИС
        2, 5, 7,                // ИЗ
        2, 28, 1,               // КО
        1, 28,                  // К
        1, 22,                  // Г
        5, 18, 3, 9, 21, 0,     // М'ЭЖДУ
        3, 16, 2, 21,           // НАД
        3, 16, 2, 27,           // НАТ
        2, 16, 2,               // НА
        2, 19, 3,               // Н'Э
        2, 1, 27,               // ОТ
        2, 1, 21,               // ОД
        2, 1, 20,               // ОБ
        2, 1, 26,               // ОП
        1, 1,                   // О
        5, 29, 3, 13, 3, 21,    // П'ЭР'ЭД
        5, 29, 3, 13, 3, 27,    // П'ЭР'ЭТ
        5, 26, 1, 35, 17, 3,    // ПОСЛ'Э
        3, 26, 1, 21,           // ПОД
        3, 26, 1, 27,           // ПОТ
        3, 26, 13, 5,           // ПР'И
        3, 26, 8, 1,            // ПРО
        2, 26, 1,               // ПО
        4, 8, 2, 24, 5,         // РАД'И
        5, 35, 28, 6, 1, 38,    // СКВОС'
        5, 35, 28, 6, 1, 12,    // СКВОЗ'
        6, 35, 11, 3, 8, 40, 2, // СВ'ЭРХА
        2, 35, 1,               // СО
        1, 35,                  // С
        5, 7, 7, 2, 24, 5,      // ЗЗАД'И
        1, 0,                   // У
        5, 33, 3, 13, 3, 35,    // ЧЭР'ЭС
        5, 33, 3, 13, 3, 7,     // ЧЭР'ЭЗ
        2, 7, 2,                // ЗА
        1, 7,                   // З
        0};

static const uint8_t seqlist3[] =
    {
        2, 20, 4,            // БЫ
        1, 20,               // Б
        2, 9, 3,             // ЖЭ
        1, 9,                // Ж
        5, 19, 5, 20, 0, 24, // Н'ИБУД'
        1, 26,               // П
        2, 27, 1,            // ТО
        0};

static const uint8_t seqlist4[] =
    {
        3, 2, 6, 2,     // АВА
        3, 3, 6, 2,     // ЭВА
        3, 2, 15, 0,    // АМУ
        3, 3, 15, 0,    // ЭМУ
        3, 5, 18, 5,    // ИМ'И
        3, 2, 10, 3,    // АЙЭ
        3, 2, 10, 2,    // АЙА
        3, 5, 10, 3,    // ИЙЭ
        3, 0, 10, 0,    // УЙУ
        4, 1, 53, 6, 2, // О-ВА
        4, 3, 6, 1, 53, // ЭВО-
        0};

static const uint8_t seqlist5[] =
    {
        2, 5, 10,     // ИЙ
        3, 1, 53, 10, // О-В
        2, 3, 10,     // ЭЙ
        2, 5, 40,     // ИШ
        2, 5, 15,     // ИМ
        3, 1, 53, 15, // О-М
        2, 3, 15,     // ЭМ
        0};

/* Local subroutines */

/*
 * Test a phoncode sequence pointed by ptr against specified list.
 * Returns non-zero when succeeded.
 */
static int test_list(const uint8_t *ptr, const uint8_t *lst)
{
  const uint8_t *item;
  for (item = lst; *item && memcmp(ptr, item + 1, *item); item += *item + 1)
    ;
  return (*item != 0) && (ptr[*item] > 42) && (ptr[*item] < 53);
}

/* Shift clause transcription one point left */
static void shift(uint8_t *buf)
{
  size_t i = 0;
  do
  {
    buf[i] = buf[i + 1];
    i++;
  } while ((buf[i] < 44) || (buf[i] > 52));
  buf[i] = 43;
}

/*
 * Advance transcription up to the specified point.
 * Returns pointer to the transcription start.
 */
static uint8_t *transcription_advance(uint8_t *transcription, uint8_t *point)
{
  if (point > (transcription + TRANSCRIPTION_START))
  {
    size_t length = (point < (transcription + TRANSCRIPTION_BUFFER_SIZE)) ? (transcription + TRANSCRIPTION_BUFFER_SIZE - point) : 0;
    if (length)
      memmove(transcription + TRANSCRIPTION_START, point, length);
    memset(transcription + TRANSCRIPTION_START + length, 43, TRANSCRIPTION_BUFFER_SIZE - TRANSCRIPTION_START - length);
  }
  return transcription + TRANSCRIPTION_START;
}

/* Synthesize speech for specified clause phonetic transcription */
static void synth_clause(uint8_t *transcription, ttscb_t *ttscb, uint8_t clause_type)
{
  //  soundscript_t *soundscript = malloc(sizeof(soundscript_t));
  //  if (soundscript)
  //    {
  time_plan_ptr_t draft;
  memset(&(ttscb->script), 0, sizeof(soundscript_t));
  ttscb->script.voice = (ttscb->flags & USE_ALTERNATIVE_VOICE) ? &female : &male;
  build_utterance(transcription, &(ttscb->script));
  draft = plan_time(transcription);
  if (draft)
  {
    apply_speechrate(&(ttscb->script), &(ttscb->timing), draft);
    free(draft);
  }
  apply_intonation(transcription, &(ttscb->script), &(ttscb->modulation), clause_type);
  if (ttscb->beforeMakeSound)
  {
    ttscb->beforeMakeSound(NULL, 0, NULL);
  }
  make_sound (&(ttscb->script), &(ttscb->wave_consumer));

  //      free(soundscript);
  //    }
}

/* Synthesize speech clause by clause for specified phonetic transcription */
static void synth(uint8_t *transcription, ttscb_t *ttscb)
{
  uint8_t *tptr;
  uint8_t *sptr = transcription + TRANSCRIPTION_START; // первые два значения в любой транскрипции - пробелы, чтобы безнаказанно брать значение "слева от себя"
  uint8_t count = 0;
  uint8_t flags = 4;

  for (tptr = transcription + TRANSCRIPTION_START; tptr < transcription + TRANSCRIPTION_BUFFER_SIZE; tptr++)
  {
    if (flags & 4)
    {
      flags &= ~4;
      if (test_list(tptr, seqlist1)) // Да, даже, едва, если, ещё много подобных слов
      // проверяем, что с текущей позиции начинается одно из них
      {
        if (flags & 1)
        {
          *sptr = 50; // пока данная "фонема" не использовалась, но одна из пауз очевидно
          synth_clause(transcription, ttscb, 0);
          tptr = transcription_advance(transcription, tptr);
          count = 0;
          flags &= ~1;
          sptr = tptr;
        }
        flags |= 2;
        continue;
      }
      else if (test_list(tptr, seqlist2)) // без, вдоль, близ, вроде
      {
        flags |= 2;
        continue;
      }
      else if (test_list(tptr, seqlist3) && // бы, б, же, нибудь,
               (tptr > (transcription + TRANSCRIPTION_START)) &&
               (*(tptr - 1) == 43))
      {
        sptr = --tptr;
        shift(sptr); // выглядит, что мы хотим убрать пробел между этим междометием, чтоб оно фактически слитно с прошлым словом
        flags &= ~2;
        continue;
      }
    }
    if (*tptr != 43)
    {
      if ((*tptr > 43) && (*tptr < 53))
      {
        // встретили знак препинания (в т.ч часть междометий, похоже, пытаемся в знак препинания 50 превратить)
        //-и сразу бежим синтезировать.
        synth_clause(transcription, ttscb, ttscb->transcription_state.clause_type);
        break;
      }
    }
    // все остальные условия подразумевают что у мы тут дошли до пробела между словами (т.е *tptr == 43)
    else if (flags & 2)
    {
      shift(tptr--);
      // то есть, все эти слова мы сливаем с последующим! Что-то я не уверен, что это хорошая идея
      flags = 4;
    }

    // это единственное место, где мы прибавляем count, в других мы его только обнуляем
    // итак, у нас пробел, слова, которое мы хотели пришпандорить к следующему, у нас не было...
    // и теперь проверяем: а может следующее слово тоже в списке?
    else if (((++count) != 3) || test_list(tptr + 1, seqlist1))
    {
      sptr = tptr;
      flags |= 5; // взвели одновременно и 1, и 5
      // т.е такое ощущение: флаг 4 нужен чтобы не устраивать проверку на слова слишком часто, т.к это затратно (пройтись по каждому списку от начала до конца!)
      // поэтому первый раз в полной неизвестности мы проверяем,
      // а потом ждём пробела, чтобы тут "взвести" для проверки.
      // и то, со странным дополнительным условием,
    }
    else
    // мы сидим на пробеле, хитрого слова до этого не было, и следующее слово тоже не хитрое
    {
      uint8_t *next;
      uint8_t perspective = 20;
      uint8_t k;
      // похоже, seqlist4 - это трёхбуквенные окончания, а seqlist5 - двухбуквенные
      if ((!test_list(tptr - 3, seqlist4)) && (!test_list(tptr - 2, seqlist5)))
      {
        sptr = tptr; // при условии, что ни одного из них не обнаружили
      }

      next = memchr(tptr + 1, 43, perspective); // ищем пробел в 20 байтах от себя
      if (next)  
      {
        // нашли
        perspective = (++next) - tptr;
      }
      // вышли на фонему сразу за ним
      else
      {
        // все 20 байтов пропустили, охренеть
        next = tptr + perspective + 1;
      }

      for (k = 1; k <= perspective; k++)
        if ((tptr[k] > 43) && (tptr[k] < 53)) // если хоть какой-то знак препинания до этого пробела - отбой
        {
          break;
        }

      if ((k > perspective) && !test_list(next, seqlist1)) // никаких знаков препинания не нашли, и последующее слово из списка 1
      {
        *sptr = 50;
        synth_clause(transcription, ttscb, 0);
        tptr = transcription_advance(transcription, sptr + 1) - 1;
        count = 0;
        flags &= ~2;
      }
      flags |= 5;
      sptr = tptr;
    }
  }
}

/* Global entry point */

/*
 * Transcription callback function.
 *
 * Finalizes provided transcription and performs speech synthesis for it.
 *
 * The buffer pointed by the first argument should contain a transcription
 * of length specified by the second argument. The last argument
 * should point to the corresponding ttscb structure.
 */
int synth_function(void *buffer, size_t length, void *attscb)
{
  ttscb_t *ttscb = attscb;
  if (length > TRANSCRIPTION_START)
  {
    if (ttscb->transcription_state.flags & CLAUSE_DONE)
    {
      ttscb->transcription_state.flags &= ~CLAUSE_DONE;
    }
    else
    {
      ((uint8_t *)buffer)[length] = 50;
      ttscb->transcription_state.clause_type = 0;
    }
    synth (buffer, ttscb);
  }
  return ttscb->wave_consumer.status;
}


