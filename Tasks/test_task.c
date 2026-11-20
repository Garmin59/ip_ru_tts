/**************************************************************
 * @file test_task.c
 * @author Ilya Petrukhin (ilya.petrukhin@gmail.com)
 * @brief 
 * @version 0.1
 * @date 04-08-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 * Test table speed is:
 * 12.8 Degree/sec = 0.223 rad/sec
 * 25.92 Degree/sec = 0.452 rad/sec
 * 49.02 Degree/sec = 0.856 rad/sec
**************************************************************/
#include <stdio.h>
#include "global.h"
#include "test_task.h"
#include "timer.h"

const char *utf8_input = "Привет, мир! 1234.5. Тёст с цифрами и знаками: ?!,.";
char koi8_output[512];

size_t utf8_to_koi8 (const char *utf8, char *koi8_buf, size_t koi8_size);



/**************************************************************
 * @brief test task
 * 
 * @param arg 
**************************************************************/
void test_task (void *arg)
{ 
  queue_data koi8_data;

  while (1)
  {
    koi8_data.buffer = koi8_output;
    koi8_data.size = utf8_to_koi8(utf8_input, koi8_output, sizeof(koi8_output));
    xQueueSend (g.tts_queue, &koi8_data, portMAX_DELAY);
    vTaskDelay (pdMS_TO_TICKS(20000));	      
  }
}

#include <stdio.h>
#include <string.h>

/*
 * Таблицы KOI8-R только для русских букв (без цифр и знаков).
 * Цифры и большинство знаков препинания в ASCII имеют одинаковые коды
 * в UTF-8 и KOI8-R, поэтому их не надо перекодировать.
 */

static const char koi8_1[48] = 
{
    0xe1, 0xe2, 0xf7, 0xe7, 0xe4, 0xe5, 0xf6, 0xfa, /* А Б В Г Д Е Ж З */
    0xe9, 0xea, 0xeb, 0xec, 0xeb, 0xee, 0xef, 0xf0, /* И Й К Л М Н О П */
    0xf2, 0xf3, 0xf4, 0xf5, 0xe6, 0xe8, 0xe3, 0xfe, /* Р С Т У Ф Х Ц Ч */
    0xfb, 0xfd, 0xff, 0xf9, 0xf8, 0xfc, 0xe0, 0xf1, /* Ш Щ Ъ Ы Ь Э Ю Я */
    0xc1, 0xc2, 0xd7, 0xc7, 0xc4, 0xc5, 0xd6, 0xda, /* а б в г д е ж з */
    0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, /* и й к л м н о п */
};

static const char koi8_2[16] = 
{
    0xd2, 0xd3, 0xd4, 0xd5, 0xc6, 0xc8, 0xc3, 0xde, /* р с т у ф х ц ч */
    0xdb, 0xdd, 0xdf, 0xd9, 0xd8, 0xdc, 0xc0, 0xd1, /* ш щ ъ ы ь э ю я */
};

size_t utf8_to_koi8 (const char *utf8, char *koi8_buf, size_t koi8_size)
{
  size_t i = 0;
  size_t j = 0;
  int matched = 0;
  uint8_t b1;
  uint8_t b2;
  uint8_t index;
  uint16_t code;
  uint8_t len_utf8 = strlen(utf8) - 1;

  while (utf8[i] != '\0' && j < koi8_size)
  {
    b1 = (uint8_t)utf8[i];

    /* ASCII: цифры, знаки препинания, латинские буквы — оставляем как есть */
    if (b1 < 0x80)
    {
      koi8_buf[j++] = (char)b1;
      i++;
      continue;
    }

    /* 2-байтовая последовательность UTF-8 (сюда попадает кириллица) */
    if ((b1 & 0xFE) == 0xD0)
    {
      if (i >= len_utf8)
      {
        break;
      }
      b2 = (uint8_t)utf8[i + 1];
      code = ((uint16_t)b1 << 8) | b2;

      if (code >= 0xD090 && code <= 0xD0BF)
      {
        index = b2 - 0x90;
        koi8_buf[j++] = koi8_1[index];
        matched = 1;
      }
      else if ((code >= 0xD180) && (code <= 0xD18F))
      {
        index = b2 - 0x80;
        koi8_buf[j++] = koi8_2[index];
        matched = 1;
      }
      else if (code == 0xD081) // Ё
      {
        koi8_buf[j++] = 0xB3;
        matched = 1;
      }
      else if (code == 0xD191) // ё
      {
        koi8_buf[j++] = 0xA3;
        matched = 1;
      }

      if (matched)
      {
        i += 2;
        continue;
      }

      /* Не русская буква: копируем как есть (можно заменить на '?' или пропустить) */
      i++;
    }
    else
    {
      /* Некорректный байт — пропускаем */
      i += 2;
    }
  }

  koi8_buf[j] = '\0';
  return j;
}

// int main(void)
// {
//   const char *utf8_input = "Привет, мир! 12345. Тест с цифрами и знаками: ?!,.";
//   char koi8_output[4096];

//   size_t len = utf8_to_koi8(utf8_input, koi8_output, sizeof(koi8_output));

//   printf("Вход (UTF-8): %s\n", utf8_input);
//   printf("Выход (KOI8-R): ");
//   fwrite(koi8_output, 1, len, stdout);
//   putchar('\n');

//   return 0;
// }
