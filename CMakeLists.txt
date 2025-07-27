cmake_minimum_required(VERSION 3.16)
project(VoltQuest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Fetch raylib into /external directory
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.5
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/raylib
)
FetchContent_MakeAvailable(raylib)

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(voltquest ${SRC_FILES})

target_include_directories(voltquest PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Link raylib
target_link_libraries(voltquest PRIVATE raylib)

# Linux-specific system libraries
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(voltquest PRIVATE
        m
        pthread
        dl
        X11
        Xrandr
        Xi
        Xinerama
        Xcursor
    )
endif()

# Copy resources folder next to the built executable
add_custom_command(
    TARGET voltquest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:voltquest>/resources"
)

