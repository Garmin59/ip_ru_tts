cmake_minimum_required(VERSION 3.19) # Проверка версии CMake.

project(ru_tts
        VERSION 1.0.0)				# Название проекта

set(SOURCE_TTS 
  female.c
  intonator.c
  male.c
  new_soundproducer.c
  #soundproducer.c
  numerics.c
  sink.c
  speechrate_control.c
  synth.c
  text2speech.c
  time_planner.c
  transcription.c
  utterance.c
  )		# Установка переменной со списком исходников

include_directories (${GLOBAL_INCLUDE_DIRECTORIES})

add_definitions(${COMPILE_DEFINITIONS})
add_compile_options(${COMPILE_OPTIONS})

add_library(ru_tts STATIC ${SOURCE_TTS})# Создание статической библиотеки
target_include_directories(ru_tts PUBLIC ${GLOBAL_INCLUDE_DIRECTORIES})
