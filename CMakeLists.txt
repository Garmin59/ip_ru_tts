cmake_minimum_required(VERSION 3.19)

set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(CMAKE_SYSTEM_NAME                   Generic)
set(CMAKE_SYSTEM_PROCESSOR              arm)
set(CMAKE_CROSSCOMPILING                TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE       STATIC_LIBRARY)

#set(CMAKE_C_COMPILER            arm-none-eabi-gcc.exe)
#set(CMAKE_CXX_COMPILER          arm-none-eabi-g++.exe)
#set(CMAKE_ASM_COMPILER          arm-none-eabi-gcc.exe)
#set(CMAKE_LINKER                arm-none-eabi-ld.exe)
#set(CMAKE_READELF               arm-none-eabi-readelf.exe)
set(CMAKE_OBJCOPY               arm-none-eabi-objcopy.exe)
set(CMAKE_SIZE                  arm-none-eabi-size.exe)
set(CMAKE_OBJDUMP               arm-none-eabi-objdump.exe)

set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build.")

# Set project name to match directory name. If set othervise tasks.json and launch.json should be updated
set(PROJECT_NAME IP_MCU_voice)
project(${PROJECT_NAME} 
        LANGUAGES C CXX ASM
        VERSION 1.0.0
        DESCRIPTION "IP MCU voice")

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(ARM_TOOLCHAIN_PATH C:/GCC/ARM/15.2.1)
#set(ARM_TOOLCHAIN_PATH C:/Ilya/ARM_toolchain/11.3.1)
set(TOOLCHAIN_BIN_PATH ${ARM_TOOLCHAIN_PATH}/bin)
set(TOOLCHAIN_INC_PATH ${ARM_TOOLCHAIN_PATH}/arm-none-eabi/include)
set(TOOLCHAIN_LIB_PATH ${ARM_TOOLCHAIN_PATH}/arm-none-eabi/lib)
set(CMAKE_TOOLCHAIN_FILE ${TOOLCHAIN_BIN_PATH}/arm-none-eabi-gcc.exe)
#set(multilib_dir ${ARM_TOOLCHAIN_PATH}/lib/gcc/arm-none-eabi/11.3.1)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 19)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c17 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++19 -Wall")
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Drivers)
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Core)
set(RTOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS)
set(TASKS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Tasks)
set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Sources)

file (GLOB_RECURSE Tasks_files ${TASKS_DIR}/*.c)
file (GLOB Core_files ${CORE_DIR}/Src/*.c)

set(PROJECT_FILES 
        ${CORE_DIR}/Startup/startup_stm32g474reux.s
        ${SOURCES_DIR}/syscalls.c
        ${SOURCES_DIR}/sysmem.c
        ${Core_files}
        ${Tasks_files}
        )

set (GLOBAL_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CORE_DIR}/Inc
        ${DRIVERS_DIR}/CMSIS/Device/ST/STM32G4xx/Include
        ${DRIVERS_DIR}/CMSIS/Include
        ${DRIVERS_DIR}/STM32G4xx_EEPROM_Emul
        ${DRIVERS_DIR}/STM32G4xx_HAL_Driver/Inc
        ${DRIVERS_DIR}/STM32G4xx_HAL_Driver/Inc/Legacy
        ${DRIVERS_DIR}/ru_tts
        ${RTOS_DIR}/include
        ${RTOS_DIR}/portable
        ${TASKS_DIR}/Inc
        )

set(OUTPUT_LD_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Linker/STM32G474REUX_FLASH.ld)

set(COMPILE_OPTIONS
        -Wall
        -O0 
        -g3
        -ggdb
        -gdwarf 
        -ffunction-sections
        -fdata-sections
        -fno-exceptions
        -ffreestanding 
        -mcpu=cortex-m4 
        -mthumb 
        -mno-thumb-interwork 
        -mfpu=fpv4-sp-d16
        -mabi=aapcs
        -mfloat-abi=hard)

add_compile_options(${COMPILE_OPTIONS})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_definitions("_RELEASE_")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_definitions("_DEBUG_")
endif()

#        -D_DEBUG_ 
set(COMPILE_DEFINITIONS 
        -DSTM32G474xx 
        -DSWO_DEBUG
        -DUSE_FULL_LL_DRIVER
        -DUSE_HAL_DRIVER
        -D__VFP_FP__
        -DCMAKE_SYSTEM_NAME=Generic
        -DCMAKE_C_COMPILER=${TOOLCHAIN_BIN_PATH}/arm-none-eabi-gcc.exe
        -DCMAKE_CXX_COMPILER=${TOOLCHAIN_BIN_PATH}/arm-none-eabi-g++.exe
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_BIN_PATH}/arm-none-eabi-gcc.exe
        -DCMAKE_AR=${TOOLCHAIN_BIN_PATH}/arm-none-eabi-ar.exe
        -DCMAKE_LINKER=${TOOLCHAIN_BIN_PATH}/arm-none-eabi-ld.exe)
add_definitions(${COMPILE_DEFINITIONS})

add_executable (${PROJECT_NAME} ${PROJECT_FILES})

#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "
#        -Wl,
#        -Xlinker 
#        -Map=${BUILD_DIR}/${PROJECT_NAME}.map
#        -T${CMAKE_CURRENT_SOURCE_DIR}/Linker/STM32G474REUX_FLASH.ld
#        ")

#        Core 
target_compile_options (${PROJECT_NAME} INTERFACE ${COMPILE_OPTIONS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${COMPILE_DEFINITIONS})
target_link_libraries(${PROJECT_NAME} PRIVATE 
        EEPROM_Emul
        hal_lib
        ru_tts
        FreeRTOS
        m
        )
# 'm' for including math library explicitly

#add_subdirectory(Core)
add_subdirectory(Drivers/STM32G4xx_EEPROM_Emul)
add_subdirectory(Drivers/STM32G4xx_HAL_Driver)
add_subdirectory(Drivers/ru_tts)
add_subdirectory(FreeRTOS)

#set(CRT0_OBJ ${multilib_dir}/crt0.o)
#set(CRT_OBJ ${multilib_dir}/crti.o
#    ${multilib_dir}/crtn.o
#    ${multilib_dir}/crtfastmath.o
#    ${multilib_dir}/crtbegin.o
#    ${multilib_dir}/crtend.o)

set(CPU_FLAGS
        -mcpu=cortex-m4 
        -mthumb
        -mabi=aapcs 
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        )

set(LINKER_FLAGS 
        -lc
        -g
        -specs=nano.specs
        -Wl,--gc-sections
        -Wl,-Map=${BUILD_DIR}/${PROJECT_NAME}.map
        -u _printf_float
        )
#        -lm
#        -lnosys

#        "LINKER:--gc-sections"
#        "LINKER:-Map=${BUILD_DIR}/${PROJECT_NAME}.map"
#        "LINKER:-L${multilib_dir}"
target_link_options(${PROJECT_NAME} PRIVATE
        ${CPU_FLAGS}
        "LINKER:--print-memory-usage"
        "LINKER:-T${CMAKE_CURRENT_SOURCE_DIR}/Linker/STM32G474REUX_FLASH.ld "
        ${LINKER_FLAGS}
        -lm
        )

include(${CMAKE_DIR}/common.cmake)

target_include_directories (${PROJECT_NAME} PRIVATE ${GLOBAL_INCLUDE_DIRECTORIES})
include_directories (${GLOBAL_INCLUDE_DIRECTORIES})

target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_FILES})

print_size_of_target(${PROJECT_NAME})
generate_elf_bin(${PROJECT_NAME})
generate_elf_hex(${PROJECT_NAME})

