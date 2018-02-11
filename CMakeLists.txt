set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 3.6)
project(chip8)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(SDL2_PATH ${CMAKE_SOURCE_DIR})

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

set(SOURCE_FILES main.cpp Chip8.cpp Timer.cpp)
add_executable(chip8 ${SOURCE_FILES})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT chip8)

target_link_libraries(chip8
	#	${SDL2MAIN_LIBRARY}
        ${SDL2_LIBRARY})
		
set(SDL2_DLL ${CMAKE_SOURCE_DIR}/lib/$(PlatformShortName)/SDL2.dll)
set(GAMES ${CMAKE_SOURCE_DIR}/games)
		
message(STATUS ${SDL2_DLL})
		
add_custom_command(TARGET chip8 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL}
        $<TARGET_FILE_DIR:chip8>)
		
add_custom_command(TARGET chip8 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${GAMES}
        $<TARGET_FILE_DIR:chip8>)