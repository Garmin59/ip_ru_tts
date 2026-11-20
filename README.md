# README #

### Cmake preparing for VScode ###

1. Install stlink utils st-flash.exe like C:\Program Files (x86)\stlink\bin and add to PATH
2. Install ARM toolchain like C:\Program Files (x86)\ARM_toolchain\11.3.1\bin and to PATH
3. Install openOCD like C:\Program Files (x86)\xpack-openocd-0.12.0-1\bin and add to PATH
4. Install CMAKE like C:\Program Files\CMake\bin and add to PATH
5. Install VScode extentions: CMake, CMake Tools, Tasks, Cortex-Debug
6. Set users path in the CMakeLists.txt oh the root of project
7. Set .vscode files as sample in the "vscode_sample_settings/vscode_for_cmake.zip" and correct users paths.
6. Set visible Tasks and Cmake Tools in the status bar (bottom on the screen)
7. Select active kit "GCC arm-none-eabi" for this project
8. Select default build target [all] and press Build on the status bar
9. Tasks options:
    "CMake build" - build the project
    "flash" - only flash device
    "Full coding" - build, then coding bin to project.bin for OTA update
10. CMake task "Build [all]" clean files and make project
11. Debug (ocd/stlink)

Install ST-link server https://www.st.com/en/development-tools/st-link-server.html
For starting the GDB Server use "st-util" in the DOS-Box.

#### Versions:
2025.04.06 Init version

