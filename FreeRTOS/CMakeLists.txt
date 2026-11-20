cmake_minimum_required(VERSION 3.19) # Проверка версии CMake.

project(FreeRTOS)				# Название проекта

FILE (GLOB_RECURSE RTOS_src_files ${RTOS_DIR}/*.c)

set(SOURCE_LIB 
  portable/heap_4.c
  portable/idle_task_static_memory.c
  portable/port.c
  croutine.c
  event_groups.c
  list.c
  queue.c
  stream_buffer.c
  tasks.c
  timers.c
  )		# Установка переменной со списком исходников


add_definitions(${COMPILE_DEFINITIONS})  
add_compile_options(${COMPILE_OPTIONS})

#add_library(FreeRTOS STATIC ${RTOS_src_files})# Создание статической библиотеки
add_library(FreeRTOS STATIC ${SOURCE_LIB})# Создание статической библиотеки
target_include_directories(FreeRTOS PUBLIC ${GLOBAL_INCLUDE_DIRECTORIES})

