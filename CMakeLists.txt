# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.5)
if(WIN32)
	
else()
	set(CMAKE_C_COMPILER "gcc")
	set(CMAKE_CXX_COMPILER "g++")
endif()
project(ledfun)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
endif()

if(WIN32)
	find_package(sdl2 CONFIG REQUIRED)
	find_package(sdl2-image CONFIG REQUIRED)
else()
	find_package(SDL2  REQUIRED)
	find_package(SDL2_image  REQUIRED)
    find_package(SDL2_mixer  REQUIRED)

endif()
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

set (SRCS
    main.cc
)

add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

if(WIN32)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image)
else()
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL2::Main SDL2::Image SDL2::Mixer modplug m)
endif()

