cmake_minimum_required(VERSION 3.11)
project(voltquest)

set(CMAKE_CXX_STANDARD 17)

# Include FetchContent module
include(FetchContent)

# =====================
# Fetch raylib
# =====================
message(STATUS "Fetching raylib...")
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)
message(STATUS "Fetched raylib successfully")

# =====================
# Fetch nanosvg
# =====================
message(STATUS "Fetching nanosvg...")
FetchContent_Declare(
  nanosvg
  GIT_REPOSITORY https://github.com/memononen/nanosvg.git
)
FetchContent_MakeAvailable(nanosvg)
message(STATUS "Fetched nanosvg successfully")

# Tell compiler where to find nanosvg headers
FetchContent_GetProperties(nanosvg)
if(NOT nanosvg_POPULATED)
  FetchContent_Populate(nanosvg)
endif()
# Add include path so `#include "nanosvg.h"` works
include_directories(${nanosvg_SOURCE_DIR}/src)

# =====================
# Fetch nlohmann_json
# =====================
message(STATUS "Fetching nlohmann/json...")
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)
message(STATUS "Fetched nlohmann/json successfully")

# =====================
# Source files
# =====================
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

# =====================
# Copy resources folder next to built executable
# =====================
add_custom_command(
    TARGET voltquest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:voltquest>/resources"
)

