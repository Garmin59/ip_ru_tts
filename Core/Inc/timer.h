/*
 * timer.h
 *
 *  Created on: 13.9.2012
 *      Author: one
 */

#ifndef TIMER_H_
#define TIMER_H_

// Choose one variant for using timers
//#define BAREMETAL_TIMERS
#ifndef BAREMETAL_TIMERS
  #define FREERTOS_TIMERS
#endif

#include <stdbool.h>
#include <stdint.h>

#ifdef FREERTOS_TIMERS
  #include "FreeRTOS.h"
  #include "task.h"
#endif
   
static inline void  timer_reset     (uint32_t *timer);
static inline bool  timer_end       (uint32_t *timer, uint32_t delay);
static inline bool  timer_active    (uint32_t *timer, uint32_t delay);
static inline uint32_t	  timer_passed    (uint32_t *timer);
static inline uint32_t	  system_time			(void);
   
#ifdef BAREMETAL_TIMERS
extern volatile uint32_t system_timer;	// системный счётчик времени в мс.
#endif

/*******************************************************************************
* прошло времени с начала таймера
*******************************************************************************/
static inline uint32_t timer_passed (uint32_t *timer)
{
#ifdef BAREMETAL_TIMERS
	return (system_time () - *timer);
#else
  return (xTaskGetTickCount () - *timer);
#endif
	
}

/*******************************************************************************
* получение системного времени
*******************************************************************************/
static inline uint32_t system_time (void)
{
#ifdef BAREMETAL_TIMERS
	return (system_timer);
#else
  return (xTaskGetTickCount ());
#endif
}


/*******************************************************************************
* сброс таймера
*******************************************************************************/
static inline void timer_reset (uint32_t *timer)
{
#ifdef BAREMETAL_TIMERS
	*timer = system_time ();
#else
  *timer = xTaskGetTickCount ();
#endif   
}

/*******************************************************************************
* условие срабатывания таймера
*******************************************************************************/
static inline bool timer_end (uint32_t *timer, uint32_t delay)
{
#ifdef BAREMETAL_TIMERS
	return ((system_time () - *timer) >= delay);
#else
  return ((xTaskGetTickCount () - *timer) >= delay);
#endif  
}

/*******************************************************************************
* условие установленного таймера
*******************************************************************************/
static inline bool timer_active (uint32_t *timer, uint32_t delay)
{
#ifdef BAREMETAL_TIMERS
	return ((system_time () - *timer) < delay);
#else
  return ((xTaskGetTickCount () - *timer) < delay);
#endif
}



#endif /* TIMER_H_ */
