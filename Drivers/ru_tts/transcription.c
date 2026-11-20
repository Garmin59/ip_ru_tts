/* transcription.c -- Phonetic transcription related functions
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

#include "transcription.h"
#include "numerics.h"
#include "sink.h"


/* Local macros */
#define PAIR(a, b) ((((uint16_t)(a)) << 8) | (((uint16_t)(b)) & 0xFF))
#ifdef _MSC_VER
#define strdup(p) _strdup(p)
#endif

/* Main punctuations */
const char *punctuations = ",.;:?!-";

/* Local data */

/* Special symbols */
static const char *symbols = " ,.;:?!()-+=\"$%&*";

/* Accepted characters list */
static const char *char_list = "TNRLMDPZG^JH_WC[FOE\\UQYX]`a'-*()%\"/&$><@+=";
// ТНРЛМДПЗГЧЙХЖШЦЩФОЕЁ УЯЫЬЪЮЭ'-*()% "/&$><@+=

/* Special symbols treated as blanks */
static const char *blanks = "\t#'/<>@";

/* Internal letters representation table */
static const char *letters = "`ABCDEFGHIJKLMNOPQRSTU_VXYZWa[^]+="; // 32 буквы русского алфавита, переделанные из KOI8 в "транслит"
                                                                   // юабцдефгхийклмнопярстужвьызшэщчъ
/* Phonetic letter classes */
static const char *consonants = "JMNRL^HC[WSPFTK_ZBVDG";
// ЙМНРЛЧХЦЩШСПФТКЖЗБВДГ
static const char *vocalics = "`EI\\QUaYOA";
static const char *ndts = "NDTS";
static const char *bgdjz = "BGD_Z";

/* Phoncodes */
static const uint8_t vocal_phoncodes[] = {0, 3, 4, 1, 2};
static const uint8_t ndts_soft_phs[] = {19, 24, 30, 38}; // именно так: н', д', т', с'
static const uint8_t hard_consonant_phs[] =
    {
        10, 15, 16, 8, 14, 33, 40, 32, 39, // Й, М, Н, Р, Л, Ч, Х, Ц, Щ - эти ни глухие ни звонкие (непарные)
        36, 35, 26, 34, 27, 28,            // Ш, С, П, Ф, Т, К - здесь ряд глухих
        9, 7, 20, 6, 21, 22                // Ж, З, Б, В, Д, Г - а здесь они же звонкие.
};
static const uint8_t soft_consonant_phs[] =
    {
        10, 18, 19, 13, 17, 33, 41, 32, 39, // Й, М', Н', Р', Л', Ч, Х', Ц, Щ - часть смягчились
        36, 38, 29, 37, 30, 31,             // Ш, Сь, Пь, Фь, Ть, Кь
        9, 12, 23, 11, 24, 25               // Ж, Зь, Бь, Вь, Дь, Гь
};

/* Predefined transcription blocks */
static const uint8_t transcription_blocks[] =
    {
        3, 27, 3, 53,                                              // Т (как ТЭ-). 27=Т твёрдое, 3=Э, 53 какая-то пауза.      [0]
        3, 3, 53, 16,                                              // Н (как Э-Н). 16=Н твёрдое                               [1]
        3, 3, 53, 8,                                               // Р (как Э-Р). 8=Р твёрдое                                [2]
        3, 3, 53, 17,                                              // Л (как Э-Ль). 17=Л мягкое                               [3]
        3, 3, 53, 15,                                              // М (как Э-М), 15=М твёрдое                               [4]
        3, 21, 3, 53,                                              // Д (как ДЭ-), 21=Д твёрдое                               [5]
        3, 26, 3, 53,                                              // П (как ПЭ-), 26=П твёрдое                               [6]
        3, 7, 3, 53,                                               // З (как ЗЭ-), 7=З твёрдое                                [7]
        3, 22, 3, 53,                                              // Г (как ГЭ-), 22=Г твёрдое                               [8]
        3, 33, 3, 53,                                              // Ч (как ЧЭ-), 33=Ч твёрдое                               [9]
        10, 5, 28, 8, 2, 53, 27, 28, 2, 10, 3,                     // Й (И кра-ткое)          [10]
        3, 40, 2, 53,                                              // Х (как ХА-), 40=Х твёрдое, 2 = А.                        [11]
        3, 9, 3, 53,                                               // Ж (как ЖЭ-), 9=Ж (единственное и неповторимое),          [12]
        3, 36, 2, 53,                                              // Ш (как ША-), 36=Ш                                        [13]
        3, 32, 3, 53,                                              // Ц (как ЦЭ-), 32=Ц                                        [14]
        3, 39, 2, 53,                                              // Щ (как ЩА-), 39=Щ                                        [15]
        3, 3, 53, 34,                                              // Ф (как Э-Ф), 34=Ф                                        [16]
        2, 1, 53,                                                  // О (так и есть О-), О=1                                   [17]
        3, 10, 3, 53,                                              // Е (Й-Э-), 10=Й.                                          [18]
        3, 10, 1, 53,                                              // Ё (ЙО-),                                                 [19]
        2, 0, 53,                                                  // У- (так и есть), У=0                                     [20]
        3, 10, 2, 53,                                              // Я (ЙА-)                                                  [21]
        2, 4, 53,                                                  // Ы-, 4=Ы                                                  [22]
        13, 18, 2, 53, 40, 28, 5, 10, 43, 7, 16, 2, 53, 28,        // Ь (мя-гкий зна-к)     [23]
        14, 27, 11, 1, 53, 8, 21, 4, 10, 43, 7, 16, 2, 53, 28,     // твё-рдый зна-к)    [24]
        3, 10, 0, 53,                                              // Ю (ЙУ)                                                  [25]
        2, 3, 53,                                                  // Э (как есть)                                            [26]
        8, 28, 2, 6, 4, 53, 33, 31, 5,                             // КАВЫ-ЧКИ                                 [27]
        0,                                                         // нихрена                                                 [28]
        8, 0, 15, 16, 1, 53, 9, 5, 30,                             // УМНО ЖИТ'                                [29]
        15, 2, 27, 28, 8, 4, 53, 30, 43, 35, 28, 1, 53, 26, 28, 0, // АТКРЫ-Т' СКО-ПКУ [30]
        15, 7, 2, 28, 8, 4, 53, 30, 43, 35, 28, 1, 53, 26, 28, 0,  // ЗАКРЫ-Т' СК-ОПКУ [31]
        10, 26, 8, 2, 32, 3, 53, 16, 27, 2, 34,                    // ПРАЦЭ-НТАФ                       [32]
        8, 28, 2, 6, 4, 53, 33, 31, 5,                             // КАВЫ-ЧКИ, дубль! (несколькими словами выше точно такие же!) - видимо, открывающие и закрывающие [33]
        5, 21, 8, 1, 53, 23,                                       // ДРО-Б'                                           [34]
        10, 2, 15, 26, 3, 8, 35, 3, 53, 16, 21,                    // АМПЭРСЭНД                        [35]
        8, 21, 1, 53, 14, 2, 8, 2, 34,                             // ДО-ЛАРАФ                                 [36]
        6, 20, 1, 53, 17, 36, 3,                                   // БО-Л'ШЭ                                      [37]
        6, 18, 3, 53, 19, 36, 3,                                   // М'Э-Н'ШЭ                                     [38]
        9, 26, 2, 8, 2, 53, 22, 8, 2, 34,                          // ПАРА-ГРАФ  (а вообще собака)           [39]
        5, 29, 17, 0, 53, 35,                                      // П'Л'УС                                           [40]
        11, 8, 2, 6, 19, 2, 53, 10, 3, 27, 38, 2,                  // РАВН'А-ЙЭТЦ'А                  [41]

        4, 1, 53, 6, 2, /* 42: O+GO */         // О-ВА (если акцент на первую О)       [42]
        4, 2, 6, 1, 53, /* 43: OGO+ */         // АВО- (если акцент на вторую О)       [43]
        3, 2, 6, 2, /* 44: OGO */              // АВА  (если без акцента)              [44]
        4, 3, 53, 6, 2, /* 45: E+GO (1) */     // Э-ВА (акцент на Е),                  [45]
        4, 3, 6, 1, 53, /* 46: EGO+ (1) */     // ЭВО- (акцент на О)                   [46]
        3, 3, 6, 2, /* 47: EGO (1) */          // ЭВА   (без акцента)                  [47]
        5, 10, 3, 53, 6, 2, /* 48: E+GO (2) */ // ЙЭ-ВА                            [48]
        5, 10, 3, 6, 1, 53, /* 49: EGO+ (2) */ // ЙЭВО                             [49]
        4, 10, 3, 6, 2, /* 50: EGO (2) */      // ЙЭВА                             [50]
        3, 27, 35, 2 /* 51: TSA */             // ТСА                              [51]
};

/* Clause termination pairs */
static const uint16_t clause_terminations[] =
    {
        PAIR(',', ' '),  // ", "
        PAIR(',', ','),  // ",,"
        PAIR(',', ';'),  // ",;"
        PAIR(',', ':'),  // ",:"
        PAIR(',', '-'),  // ",-"
        PAIR('.', ' '),  // ". "
        PAIR('.', '.'),  // ".."
        PAIR(';', ' '),  // "; "
        PAIR(':', ' '),  // ": "
        PAIR('.', ':'),  // ".:"
        PAIR('?', ' '),  // "? "
        PAIR('?', '.'),  // "?."
        PAIR('?', ','),  // "?,"
        PAIR('!', ' '),  // "! "
        PAIR('!', '.'),  // "!."
        PAIR('!', ',')}; // "!,"

/* Local subroutines */

/* Put specified block to the transcription buffer */
static void put_transcription_block(sink_t *consumer, uint8_t n)
{
  transcription_state_t *transcription = consumer->user_data;
  const uint8_t *len = list_item(transcription_blocks, n);

  for (uint8_t i = 1; i <= *len; i++) // *len показывает количество элементов, сообразили.
  {
    uint8_t c = len[i];
    sink_put(consumer, ((c == 53) && (transcription->flags & WEAK_STRESS)) ? 54 : c);
  }
}

/*
 * Detect specified suffix in the input text
 * and make all necessary changes in the referenced control data.
 *
 * Returns non-zero value when the match succeeded.
 */
static int detect_suffix(input_t *input, transcription_state_t *transcription, const char *suffix)
{
  int rc = 0;
  uint8_t n = strlen(suffix);
  uint8_t i;

  transcription->flags &= ~WEAK_STRESS;
  for (i = 0; i < n; i++)
    if (input->start[i] != suffix[i])
    {
      if ((suffix[i] == '+') && (input->start[i] == '='))
        transcription->flags |= WEAK_STRESS;
      else
        break;
    }

  if ((i == n) && (((input->start + n) >= input->end) || !strchr(letters, input->start[n])))
  {
    input->start += n - 1;
    rc = 1;
  }

  return rc;
}

/*
 * Check if clause transcription is terminated. If it is the case,
 * terminates transcription and flushes it to the consumer.
 * Returns non-zero value on success.
 */
static int check_clause_termination(input_t *input, sink_t *consumer)
{
  transcription_state_t *transcription = consumer->user_data;
  char *s = memchr(symbols + 1, *(input->start), 6); // ,.;:?!
  int result = 0;

  if (s)
  {
    char c = *(input->start++);
    char nextc = (input->start < input->end) ? *(input->start) : ' ';
    uint16_t termination = PAIR(c, strchr(punctuations, nextc) ? nextc : ' ');
    uint8_t i;
    for (i = 0; (i < (sizeof(clause_terminations) / sizeof(uint16_t))) && (clause_terminations[i] != termination); i++)
      {

      }
    transcription->clause_type = i & 0x0F;
    transcription->flags |= CLAUSE_DONE;
    sink_put(consumer, s - symbols + 43);
    // т.е
    // 43: пробел
    // 44: запятая
    // 45: точка
    // 46: точка с запятой
    // 47: двоеточие
    // 48: вопросительный знак
    // 49: восклицательный знак
    sink_flush(consumer);
    result = 1;
  }

  return result;
}

/*
 * Get consonant phoncode by index from specified set
 * voicifying the unvoiced ones.
 */
// говоря русским языком: из глухих делаем звонкие. Первые 9 у нас непарные (всякие Й, Л, М, Н ...),
// затем в каждом списке (мягких и твёрдых согласных) идут глухие, за ними звонкие.
// хотя бред какой-то. Да, глухие станут звонкими, но непарные превратятся не пойми во что!
// возможно, это условие проверяется раньше...
// да, норм: первые 9 согласных, которые непарные, рассматриваются особо. Это функция для них не вызовется ни в жисть
static uint8_t voicify(const uint8_t *phs, uint8_t idx)
{
  return phs[(idx < 15) ? (idx + 6) : idx];
}

/*
 * Get consonant phoncode by index from specified set
 * unvoicifying the voiced ones.
 */
// превратить звонкие в глухие.
// вот тут всё верно: только звонкие поменяются, а глухие и непарные останутся как есть.
static uint8_t unvoicify(const uint8_t *phs, uint8_t idx)
{
  return phs[(idx < 15) ? idx : (idx - 6)];
}

/*
 * Get hard consonant phoncode by its index
 * unvoicifying the voiced ones and changing if necessary
 * in accordance with the following character.
 */
// конкретно твёрдые согласные заглушить
// особый случай: С или З, а следующая буква Ш. Тогда ставим звук Ш.
// примеры: расшитый, низший
// точнее так: сделать и твёрдой, и глухой.
static uint8_t unvoicify_hard(uint8_t idx, char following)
{
  return (((idx != 10) && (idx != 16)) || (following != 'W')) ? unvoicify(hard_consonant_phs, idx) : 36;
}

/*
 * Get correct consonant phoncode by its index
 * depending on the following character specified.
 */
static uint8_t correct_consonant(uint8_t idx, char following)
{
  if (memchr(consonants + 5, following, 10))                      // проверяем, является ли следующая буква одной из 10 согласных: ХЦЩШСПФТКЖ
    return unvoicify_hard(idx, following);                        // и ежели так, предыдущую делаем глухой и твёрдой. Хотя затвердение не увидел почему-то.
  else if (strchr(bgdjz, following))                              // это, что ль, и есть все звонкие, кроме [в], [в’], [й’], [л], [л’], [м], [м’], [н], [н’], [р], [р’]?
    return (((idx != 10) && (idx != 16)) || (following != '_')) ? // особый случай: опять с,з, а за ними Ж. Тогда и с/з превращаем в Ж
               voicify(hard_consonant_phs, idx)
                                                                : 9; // например, сжиженный газ. Обычный случай: озвончаем и как будто выдаём твёрдый звук.
  return ((idx != 16) || (following != '_')) ?                       // и опять вроде как твёрдую согласную выдаём (видимо потом дополнительно ещё смягчим при необходимости),
             hard_consonant_phs[idx]
                                             : 9; // и особый случай: з, за которой идёт Ж. Не понимаю: мы ж это проверяли уже! У нас если Ж, просто сюда управление не перейдёт!
}

/* Transcription cycle initialization actions */
static void transcription_init(sink_t *consumer)
{
  uint8_t *buffer = consumer->buffer;
  memset(buffer, 43, TRANSCRIPTION_BUFFER_SIZE); // по умолчанию сплошные паузы
  consumer->buffer_offset = TRANSCRIPTION_START;
}

/* Common functions */

/* Get an item from the list by its number */
// список "вшитых слов" (как произносить буквы по одной, ВЭ, ГЭ, ДЭ, И КРАТКОЕ, кавычки, долларов, равняется, плюс и пр.)
// в нём каждое слово начинается с байта размера слова, затем идёт само слово, и так далее.
// поэтому, чтобы выбрать слово под номером N, нужно пройтись от самого первого, прибавляя по длине каждого слова и ещё 1 за счёт байта размера
const uint8_t *list_item(const uint8_t *lst, uint8_t n)
{
  const uint8_t *len = lst;

  for (uint8_t i = 0; i < n; i++)
  {
    len += *len + 1;
  }
  return len;
}

/* Transcribe specified text clause by clause and pass result to the consumer */
void process_text (const char *text, sink_t *consumer)
{
  input_t input;
  transcription_state_t *transcription = consumer->user_data;
  char *s;
  int accented = 0;

  consumer->custom_reset = transcription_init;
  input.text = strdup(text);
  if (!input.text)
    return;

  input.start = input.text;
  input.end = input.text;
  transcription->flags = 0;

  for (s = input.text; *s; s++)
  {
    unsigned char c = *s;

    switch (c)
    {
    case '\n':
    case '\r':
      c = '\r';
      break;

    case 'j':
    case 'J':
      c = '_'; // типа, транслит от КОИ8 всё ж отличается. j это не й (как в нём) а будем считать Ж. (ну пущай)
      break;

    case 'q':
    case 'Q':
    case 'x':
    case 'X':
      c = 'K'; // q,Q стал бы Я (в КОИ8), а x,X и вовсе мягким знаком.
      break;

    case 'w':
    case 'W': // w в КОИ8 стал бы Ш (чисто по начертанию видимо), а для транслита хотим U.
      c = 'U';
      break;
    case 'y': // в КОИ8 стал бы Ы, а мы хотим И.
    case 'Y':
      c = 'I';
      break;

    case 163:
    case 179:
      c = '\\'; // буква Ё
      break;

    default:
      if (strchr(blanks, c))
      {
        c = ' ';
      }
      else if (c > 191) // русские буквы в КОИ8, сбрасываем регистр, все заглавные.
      {
        c = letters[(c - 192) & 31]; // чего-то переусложнили: вроде можно было чисто математически, без массива.
      }
      else if ((c >= 'a') && (c <= 'z'))
      {
        c -= 0x20; // строчный транслит (ранее не превращённый) превращаем в транслит заглавный
      }
      else if (((c < 'A') && !strchr(symbols, c) && !IS_DIGIT(c)) || (c > 'Z'))
      {
        c = 0; // какая-то хрень (не русские, не латинские, не цифры, не допустимые символы)
      }
      break;
    }

    if (c) // сразу же не сдались.
    {
      const char *sptr = strchr(symbols, c); // символы это ,.;:?!()-+=\"$%&* (включая пробел, он тут первый в списке)
      if (sptr)
      {
        int sidx = sptr - symbols;
        unsigned char nextc = s[1];
        if ((sidx > 6) ||                             // если скобки, плюс минус равно кавычка процент амперсанд звёздочка - условие выполнено
            ((transcription->flags & CLAUSE_START) && // либо если хоть одна буква уже была, при этом следующий символ буква или цифра или перенос строки, либо если это не пробел.
             ((c != ' ') || (nextc == '\r') ||
              IS_DIGIT(nextc) || (nextc >= 'A')))) // выходит это мы от пробелов так мучительно избавляемся?
          *(input.end)++ = c;
      }
      else
      {
        *(input.end)++ = c;
        transcription->flags |= CLAUSE_START;
      }
    }
  }
  *(input.end) = 0;

  for (s = input.start; (s < input.end) && (*s < 'A') && !IS_DIGIT(*s); s++)
  {
    ; // проверяем на предмет: есть ли вообще хоть одна буква или цифра по итогу? Если нет, сразу сдаёмся.
  }

  if (s >= input.end)
  {
    free(input.text);
    return;
  }

  while ((input.start < input.end) && !consumer->status)
  {
    unsigned char last_char = 0;

    while ((input.start < input.end) && memchr(symbols, *(input.start), 7)) // пропускаем в начале транскрипции символы ",.;:?!"
    {
      input.start++;
    }
    sink_reset(consumer);
    for (transcription->flags = CLAUSE_START; (input.start < input.end) && (consumer->buffer_offset < TRANSCRIPTION_MAXLEN) && !consumer->status; input.start++)
    {
      char *s;
      unsigned char c = *(input.start);

      if (transcription->flags & CLAUSE_START)
      {
        accented = 0;
        for (s = input.start; s < input.end; s++)
          if ((*s == '+') || (*s == '='))
          {
            accented = 1;
            break;
          }
          else if (*s < 'A')
            break;
      } // проверяем акценты, покуда работаем с буквами. Как дошли до чего-то ещё (пробелы, цифры, может знаки препинания) - усё, бросаем на полпути.

      s = strchr(char_list, c); // самое загадочное - этот char_list он же нихрена не все наши буквы содержит!
      // но именно для них проверяются некие частные случаи.
      if (s)
      {
        uint8_t char_index = s - char_list;
        if ((char_index < 17) &&    // Т, Н, Р, Л, М, Д, П, З, Г, Ч, Й, Х, Ж, Ш, Ц, Щ, Ф, т.е все  согласные из этого списка
            (last_char != '+') &&   // не было акцента
            (last_char != '=') &&   // либо слабого акцента (на длинных словах)
            (last_char < 'A') &&    // и вообще предыдущий символ был не буквой
            (input.start[1] < 'A')) // да и последующий тоже
        {
          put_transcription_block(consumer, char_index); // не одиночный звук тащим, а прям целый набор звуков, и это первые 17 строк transcription_block.
          transcription->flags |= CLAUSE_START;
          last_char = c;
          continue;
        }
        else if (char_index > 26) //%, ", /, &, $, >, <, @, +, =
        {
          uint8_t prev = sink_last(consumer);              // prev - это не буква, это фонема!
          if (((c != '+') && (c != '=')) || (prev > 5))    // 0..5 - это У, О, А, Э, Ы, И - все гласные звуки.
          {                                                // т.е здесь исключили случай расстановки ударения.
            if ((prev < 43) || (prev > 52))                // похоже, что от 43 до 52 - это паузы разной длительности. (хотя 53 и 54 тоже пауза, но совсем короткая)
              sink_put(consumer, 43);                      // т.е если не было паузы - добавим её.
            put_transcription_block(consumer, char_index); // озвучивание этих символов (процент, кавычки, дробь, ампэрсэнд, больше, меньше, насчёт собаки не уверен)
            // собака здесь это параграф
            if ((c != '-') && (input.start[1] >= 'A')) // первое условие выполняется всегда - минуса тут получиться не может.
              sink_put(consumer, 43);                  // выходит: если за этим символом идёт буква, то сделаем паузу.
            transcription->flags |= CLAUSE_START;
          }
          else
          {                                           // на самом деле мы ударение ставим
            sink_put(consumer, (c != '+') ? 54 : 53); // похоже, 53 подлиннее (основное ударение), 54 покороче (дополнительное на длинном слове)
            transcription->flags &= ~CLAUSE_START;
          }
          last_char = c;
          continue;
        }
      }

      /* Some suffixes are transcribed specially when accented */
      if (accented)
      {
        if (detect_suffix(&input, transcription, "O+GO"))
        {
          put_transcription_block(consumer, 42);
          last_char = 'A';
          continue;
        }
        else if (detect_suffix(&input, transcription, "E+GO"))
        {
          s = input.start - 4;
          put_transcription_block(consumer, ((s >= input.text) && strchr(consonants, *s)) ? 45 : 48);
          last_char = 'A';
          continue;
        }
        else if (detect_suffix(&input, transcription, "EGO+"))
        {
          s = input.start - 4;
          put_transcription_block(consumer, ((s >= input.text) && strchr(consonants, *s)) ? 46 : 49);
          last_char = 'O';
          continue;
        }
        else if (detect_suffix(&input, transcription, "OGO+"))
        {
          put_transcription_block(consumer, 43);
          last_char = 'O';
          continue;
        }
        else if (detect_suffix(&input, transcription, "EGO"))
        {
          s = input.start - 3;
          put_transcription_block(consumer, ((s >= input.text) && strchr(consonants, *s)) ? 47 : 50);
          last_char = 'A';
          continue;
        }
        else if (detect_suffix(&input, transcription, "OGO"))
        {
          put_transcription_block(consumer, 44);
          last_char = 'A';
          continue;
        }
        else if (detect_suffix(&input, transcription, "TSQ")) // ТСЯ
        {
          put_transcription_block(consumer, 51);
          last_char = 'A';
          continue;
        }
        else if (detect_suffix(&input, transcription, "TXSQ")) // ТЬСЯ - в транскрипцию одинаково попадают.
        {
          put_transcription_block(consumer, 51);
          last_char = 'A';
          continue;
        }
      }

      if (IS_DIGIT(c))
      {
        sink_flush(consumer);
        process_number(&input, consumer);
        if (check_clause_termination(&input, consumer))
          transcription->flags |= CLAUSE_START;
        else
          sink_flush(consumer);
        last_char = ' ';
        if (*(input.start) != ' ')
          input.start--;
        continue;
      }

      if (check_clause_termination(&input, consumer))
        break;

      s = strchr(vocalics, c);
      if (s)
      {
        uint8_t vc = (c == 'I') ? 5 : (((c == 'O') && accented && (input.start[1] != '+') && (input.start[1] != '=')) ? 2 : vocal_phoncodes[(s - vocalics) % 5]);
        transcription->flags &= ~CLAUSE_START;
        if (input.start > input.text)
        {
          unsigned char prevc = *(input.start - 1);
          if (prevc != 'X') // во-первых, странно, что не применили last_char (может, там мягкий знак пропустился). Но вообще на мягкий знак перед гласной проверяем.
          {
            if ((strchr(vocalics, prevc) || memchr(symbols, prevc, 13) ||
                 (prevc == ']')) &&
                strchr("`QE\\", c))
              sink_put(consumer, 10); // и краткое всунули
          }
          else if (strchr("`QE\\IO", c))
            sink_put(consumer, 10); // и здесь и краткое
        }
        else if (strchr("`QE\\", c))
          sink_put(consumer, 10); // и снова и краткое
        sink_put(consumer, vc);   // какая-то из гласных
        last_char = c;
        continue;
      }

      if (strchr(ndts, c))
      {
        unsigned char nextc = input.start[1];
        if (memchr(vocalics, nextc, 5) || (nextc == 'X'))
        {
          s = strchr(ndts, last_char);
          if (s)
            sink_replace(consumer, ndts_soft_phs[s - ndts]);
        }
      }

      s = strchr(consonants, c);
      if (s)
      {
        unsigned char nextc = ((input.end - input.start) > 1) ? input.start[1] : ',';
        uint8_t idx = s - consonants;
        transcription->flags &= ~CLAUSE_START;
        if (idx < 9) // т.е Й, М, Н, Р, Л, Ж, Х, Ц, Щ
        {
          if (nextc == 'X')
          {
            input.start++;
            sink_put(consumer, soft_consonant_phs[idx]);
          }
          else
            sink_put(consumer, (memchr(vocalics, nextc, 5) ? soft_consonant_phs : hard_consonant_phs)[idx]);
        }
        else if (nextc == 'X') // это у нас мягкий знак!
        {
          input.start++;
          nextc = ((input.end - input.start) > 1) ? input.start[1] : ',';
          if ((memchr(symbols + 1, nextc, 6) && (sink_last(consumer) != 43)) || memchr(consonants + 5, nextc, 10))
            sink_put(consumer, unvoicify(soft_consonant_phs, idx));
          else if (strchr(bgdjz, nextc))
            sink_put(consumer, voicify(soft_consonant_phs, idx));
          else
            sink_put(consumer, soft_consonant_phs[idx]);
        }
        else if (memchr(vocalics, nextc, 5)) // за нашей согласной следует гласная
        {
          sink_put(consumer, soft_consonant_phs[idx]);
        }
        else if (memchr(symbols + 1, nextc, 6)) // за нашей согласной следует знак препинания, не пробел (т.е  ,.;:?!)
        {
          sink_put(consumer, (sink_last(consumer) != 43) ? unvoicify_hard(idx, nextc) : hard_consonant_phs[idx]);
        }
        else
        {
          sink_put(consumer, correct_consonant(idx, (nextc != ' ') ? nextc : input.start[2]));
        }
      }
      else if (c != ']')
      {
        transcription->flags |= CLAUSE_START;
        sink_put(consumer, (c != '#') ? 43 : 42); // пауза, подлиннее или покороче, примерно соотв. пробелу
      }
      else
      {
        transcription->flags &= ~CLAUSE_START;
      }
      last_char = c;
    }
    sink_flush(consumer);
  }
  free(input.text);
}
