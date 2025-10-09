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

if(EMSCRIPTEN)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
  set(BUILD_STATIC OFF CACHE BOOL "" FORCE)
  set(BUILD_PTHREADS OFF CACHE BOOL "Disable pthreads for web" FORCE)
  set(USE_GLFW ON CACHE BOOL "" FORCE)
  add_compile_definitions(NO_PTHREADS)
endif()

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

# =====================
# Emscripten / Web support
# =====================
if(EMSCRIPTEN)
  message(STATUS "Building for WebAssembly / Emscripten")
  # Output as HTML
  set_target_properties(voltquest PROPERTIES SUFFIX ".html")

  # Define link flags
  set(EMSCRIPTEN_FLAGS
    "-O3"
    "-s USE_GLFW=3"
    "-s ASYNCIFY"
    "-s FULL_ES2=1"
    "-s WASM=1"
    "-s MINIFY_HTML=0"
    "-s ALLOW_MEMORY_GROWTH=1"
    "-s EXPORTED_FUNCTIONS=['_free','_malloc','_main'] "
    "-s EXPORTED_RUNTIME_METHODS=ccall"
    "--shell-file ${raylib_SOURCE_DIR}/src/shell.html"
  )

  # Add resource preloading
  set(RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
  list(APPEND EMSCRIPTEN_FLAGS "--preload-file ${RESOURCE_DIR}@/resources")

  # Convert list to string and set
  string(REPLACE ";" " " EMSCRIPTEN_FLAGS_STR "${EMSCRIPTEN_FLAGS}")
  set_target_properties(voltquest PROPERTIES LINK_FLAGS "${EMSCRIPTEN_FLAGS_STR}")
endif()

