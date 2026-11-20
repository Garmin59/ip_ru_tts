cmake_minimum_required(VERSION 3.19) # Проверка версии CMake.
project(EEPROM_Emul_library
    VERSION 1.0.0)				# Название проекта

set(EEPROM_SOURCES 
    eeprom_emul.c
    flash_interface.c)		# Установка переменной со списком исходников

set (EEPROM_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DRIVERS_DIR}/CMSIS/Device/ST/STM32G4xx/Include
    ${DRIVERS_DIR}/CMSIS/Include
    ${DRIVERS_DIR}/STM32G4xx_HAL_Driver/Inc
    ${DRIVERS_DIR}/STM32G4xx_HAL_Driver/Inc/Legacy)

add_definitions(${COMPILE_DEFINITIONS})
add_compile_options(${COMPILE_OPTIONS})

add_library(EEPROM_Emul STATIC ${EEPROM_SOURCES})# Создание статической библиотеки

target_include_directories(EEPROM_Emul PUBLIC ${GLOBAL_INCLUDE_DIRECTORIES})
