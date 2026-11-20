#include <string.h>
#include "timer.h"
#include "global.h"
#include "button_task.h"
#include "event_groups.h"

bool btn_debounce (void);
void button_proc (void);

static const uint32_t	BTN_POLL_TIME	      = 10;	  // период опроса кнопок
 

/******************************************************************************
 * @brief button_task
 * 
 * @param arg 
******************************************************************************/
void button_task (void *arg)
{
  while (1)
  {
    button_proc ();      
    vTaskDelay (pdMS_TO_TICKS(BTN_POLL_TIME));	
  }
}


/*****************************************************************************
* Опрос кнопки и анидребезг
* вход - состояние пина кнопки
* выход - переменная btn
*****************************************************************************/
bool btn_debounce (void)
{
  static const uint32_t  BTN_DEBOUNCE  = 50;  // время антидребезга
  static uint32_t	      btn_timer	    = 0;
  static bool btn = false;  // состояние кнопки
  
	// опросить состояние кнопки, записать и перевести вывод в состояние ОС
	if (btn)	// если состояние - кнопка нажата
	{
		if (BLUE_BUTTON_PIN)					// если не досчитали и нажали кнопку
		{
			timer_reset (&btn_timer);	// тогда сбрасываем таймер и считаем заново
		}
    else			// при отпускании кнопки
		{
			if (timer_end (&btn_timer, BTN_DEBOUNCE))  // считаем время, которое отжата для антидребезга
			{
        timer_reset (&btn_timer);	// как досчитали - сбросили таймер
				btn = false;	          // и переключились
			}
		}
	}
	else		// если состояние - кнопка не нажата
	{
		if (BLUE_BUTTON_PIN)					// когда нажали кнопку
		{
			if (timer_end (&btn_timer, BTN_DEBOUNCE))// считаем время, которое нажата для антидребезга
			{
				timer_reset (&btn_timer);	// как досчитали - сбросили таймер
				btn = true;		          // и переключились
			}
		}	
    else			// если кнопка отпущена
		{
			timer_reset (&btn_timer);		// тогда сбрасываем таймер и считаем заново
		}
	}
  return btn;
}


/*****************************************************************************
* Опрос кнопки и анидребезг
* вход - состояние пина кнопки
* выход - сообщения
*****************************************************************************/
void button_proc (void)
{
  typedef enum _button_states
  {
    BTN_OFF = 0,
    BTN_ON1,
    BTN_WAIT_ON2,
    BTN_ON2,
    BTN_WAIT_OFF
  } button_states;

  static uint32_t btn_timer      = 0;
  static const uint32_t  BTN_LONG_PRESS_TIME = 1100;	// время длинного нажатия на кнопку
  static const uint32_t  BTN_WAIT_ON2_TIME   = 500;	// время ожидания повторного нажатия на кнопку

	static button_states	btn_state	= BTN_OFF;
  bool btn;
  

  btn = btn_debounce (); // Антидребезг кнопки

  switch (btn_state)
  {
  case BTN_OFF:// кнопка не нажата
    if (btn)
    {
      timer_reset (&btn_timer);
      btn_state = BTN_ON1;
    }
    break;
    
  case BTN_ON1:
    if (!btn)
    {
      btn_state = BTN_WAIT_ON2;
      timer_reset (&btn_timer);
    }
    else if (timer_end (&btn_timer, BTN_LONG_PRESS_TIME))
    {
      btn_state = BTN_WAIT_OFF;
      timer_reset (&btn_timer);
      xEventGroupSetBits (g.events, EVENT_BUTTON_LONG_PRESS);
      // send_message (BTN_LONG_PRESS);
    }
    break;
  
  case BTN_WAIT_ON2:
    if (btn)
    {
      timer_reset (&btn_timer);
      btn_state = BTN_WAIT_OFF;
      xEventGroupSetBits (g.events, EVENT_BUTTON_DOUBLE_PRESS);
      // send_message (BTN_DOUBLE_PRESS);
    }
    else if (timer_end (&btn_timer, BTN_WAIT_ON2_TIME))
    {
      btn_state = BTN_OFF;
      xEventGroupSetBits (g.events, EVENT_BUTTON_SHORT_PRESS);
      xEventGroupSetBits (g.events, EVENT_BUTTON_RELEAZE);
      // send_message (BTN_SHORT_PRESS);
      // send_message (BTN_RELEASE);
    }
    break;
    
  case BTN_WAIT_OFF:
    if (!btn)
    {
      btn_state = BTN_OFF;
      xEventGroupSetBits (g.events, EVENT_BUTTON_RELEAZE);
      // send_message (BTN_RELEASE);
    }
    break;
    
  default:
    btn_state = BTN_OFF;
    break;	
  }
}

