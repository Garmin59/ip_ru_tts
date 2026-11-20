cmake_minimum_required(VERSION 3.19) # Проверка версии CMake.

project(hal_lib
        VERSION 1.0.0)				# Название проекта

set(SOURCE_HAL 
  Src/stm32g4xx_hal.c
  Src/stm32g4xx_hal_cortex.c
  #Src/stm32g4xx_hal_dma_ex.c
  #Src/stm32g4xx_hal_dma.c
  Src/stm32g4xx_hal_exti.c
  Src/stm32g4xx_hal_fdcan.c
  Src/stm32g4xx_hal_flash_ex.c
  Src/stm32g4xx_hal_flash_ramfunc.c
  Src/stm32g4xx_hal_flash.c
  #Src/stm32g4xx_hal_gpio.c
  #Src/stm32g4xx_hal_i2c.c
  #Src/stm32g4xx_hal_i2c_ex.c 
  #Src/stm32g4xx_hal_pwr.c
  #Src/stm32g4xx_hal_pwr_ex.c
  Src/stm32g4xx_hal_rcc.c
  Src/stm32g4xx_hal_rcc_ex.c
  #Src/stm32g4xx_hal_tim.c
  #Src/stm32g4xx_hal_tim_ex.c 
  #Src/stm32g4xx_hal_uart.c
  #Src/stm32g4xx_hal_uart_ex.c
  #Src/stm32g4xx_ll_adc.c
  #Src/stm32g4xx_ll_cordic.c
  #Src/stm32g4xx_ll_crc.c
  #Src/stm32g4xx_ll_dma.c
  #Src/stm32g4xx_ll_exti.c
  #Src/stm32g4xx_ll_gpio.c
  #Src/stm32g4xx_ll_i2c.c
  #Src/stm32g4xx_ll_rcc.c
  #Src/stm32g4xx_ll_rng.c
  #Src/stm32g4xx_ll_tim.c
  #Src/stm32g4xx_ll_usart.c
  Src/stm32g4xx_ll_utils.c
  )		# Установка переменной со списком исходников

include_directories (${GLOBAL_INCLUDE_DIRECTORIES})

add_definitions(${COMPILE_DEFINITIONS})
add_compile_options(${COMPILE_OPTIONS})

add_library(hal_lib STATIC ${SOURCE_HAL})# Создание статической библиотеки
target_include_directories(hal_lib PUBLIC ${GLOBAL_INCLUDE_DIRECTORIES})
