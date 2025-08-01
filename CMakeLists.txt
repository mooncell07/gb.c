cmake_minimum_required(VERSION 3.22)
project(reGB VERSION 2.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

find_package(SDL3 CONFIG REQUIRED)

if(NOT SDL3_FOUND)
    message(FATAL_ERROR "SDL3 not found!")
endif()

set(SOURCE_FILES
    log.c/src/log.c
    
    src/cartridge.c
    src/bus.c
    src/joypad.c
    src/mmio.c
    src/mappers.c
    src/clock.c
    src/timer.c
    src/CPU/backend.c
    src/CPU/cpu.c
    src/PPU/fetcher.c
    src/PPU/vbus.c
    src/PPU/ppu.c
    src/PPU/lcd.c
    src/PPU/fifo.c
    src/main.c
)

set(
    TEST_FILES

    tests/json_test_main.c
    src/CPU/backend.c
    src/bus.c
)

include_directories(include)
include_directories(log.c/src/)

if(NOT CMAKE_BUILD_TYPE MATCHES Test)

add_executable(reGB ${SOURCE_FILES} include)
target_include_directories(reGB PRIVATE include)
target_link_libraries(reGB PUBLIC SDL3::SDL3)

if (CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(reGB PRIVATE -O3 -DLOG_USE_COLOR)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(reGB PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Og -DLOG_USE_COLOR)
endif()

else()
    add_library(reGB SHARED ${TEST_FILES})
    target_compile_options(reGB PRIVATE -DDEBUG)
endif()
