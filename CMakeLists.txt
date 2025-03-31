cmake_minimum_required(VERSION 3.21)
project(gbc VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file" FORCE)
set(CMAKE_BUILD_TYPE Debug)

find_package(SDL3 CONFIG REQUIRED)
if(NOT SDL3_FOUND)
    message(FATAL_ERROR "SDL3 not found!")
endif()

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*c")

add_library(core STATIC ${SOURCE_FILES})
target_link_libraries(core PRIVATE SDL3::SDL3)
target_include_directories(core PUBLIC include)

add_executable(gbc main.c)
target_link_libraries(gbc PRIVATE core)

if (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=undefined -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()


