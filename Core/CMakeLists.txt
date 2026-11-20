cmake_minimum_required(VERSION 3.19) # Проверка версии CMake.

project(Core)				# Название проекта

file (GLOB_RECURSE CoreSrcFiles ${CORE_DIR}/*.c)

# set(SOURCE_LIB 
#   Startup/startup_stm32g474reux.s
#   Src/bootloader.c
#   Src/clocks.c
#   Src/crc.c
#   Src/error.c
#   Src/fdcan.c
#   Src/flash.c
#   Src/gpio.c
#   Src/rng.c
#   Src/stm32g4xx_hal_msp.c
#   Src/stm32g4xx_it.c
#   Src/system_stm32g4xx.c
#   Src/usart.c
#   )		# Установка переменной со списком исходников

#add_subdirectory(Src)


add_definitions(${COMPILE_DEFINITIONS})  
add_compile_options(${COMPILE_OPTIONS})

add_library(Core STATIC ${CoreSrcFiles})# Создание статической библиотеки
target_include_directories(Core PUBLIC ${GLOBAL_INCLUDE_DIRECTORIES})

#target_link_libraries(Core PUBLIC CoreSrc)
