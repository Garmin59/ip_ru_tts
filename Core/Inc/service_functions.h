/***
* @file service_functions.h
***/

#ifndef __SERVICE_FUNCTIONS_H__
#define __SERVICE_FUNCTIONS_H__

// ────────────────────────────────────────────────────────────────────────────────
#include "stdint.h"
#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
/*******************************************************************************
* Simple Inline Software TIM
*******************************************************************************/
static inline void      ms_timer_reset  (uint32_t *timer);
static inline uint32_t  ms_timer_value  (uint32_t *timer);
static inline bool      ms_timer_end    (uint32_t *timer, uint32_t delay);
static inline bool      ms_timer_active (uint32_t *timer, uint32_t delay);

/*******************************************************************************
* @brief TIM Reset
*******************************************************************************/
static inline void ms_timer_reset (uint32_t *timer)
{
  *timer = xTaskGetTickCount ();
}


/*******************************************************************************
* @brief time after start timer
*******************************************************************************/
static inline uint32_t ms_timer_value (uint32_t *timer)
{
  return (pdTICKS_TO_MS (xTaskGetTickCount () - *timer));
}

/*******************************************************************************
* @brief check TIM End condition
*******************************************************************************/
static inline bool ms_timer_end (uint32_t *timer, uint32_t delay)
{
  return ((xTaskGetTickCount () - *timer) >= pdMS_TO_TICKS (delay));
}

/*******************************************************************************
* @brief check if TIM still active
*******************************************************************************/
static inline bool ms_timer_active (uint32_t *timer, uint32_t delay)
{
  return ((xTaskGetTickCount () - *timer) < pdMS_TO_TICKS(delay));
}





// ────────────────────────────────────────────────────────────────────────────────
//
// ───────────────────────────────────────── END OF SERVICE FUNCTIONS FILE ─────
//
// ────────────────────────────────────────────────────────────────────────────────
#endif // __SERVICE_FUNCTIONS_H__
