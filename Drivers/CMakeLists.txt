add_subdirectory(STM32G4xx_EEPROM_Emul)
add_subdirectory(STM32G4xx_HAL_Driver)
add_subdirectory(ru_tts)

#set(SOURCE_HAL "")
#add_library(Drivers STATIC ${SOURCE_HAL})# Создание статической библиотеки
#target_include_directories(Drivers PUBLIC ${GLOBAL_INCLUDE_DIRECTORIES})

target_link_libraries(
  EEPROM_Emul 
  hal_lib
  ru_tts)

