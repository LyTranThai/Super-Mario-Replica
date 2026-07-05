cmake_minimum_required(VERSION 3.12)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(MarioGame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure FetchContent to automatically download and build Raylib from source
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)

# Customize raylib build configuration (disable unused targets to speed up compilation)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(USE_AUDIO ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# Gather source and header files recursively
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(MarioGame main.cpp ${SOURCES} ${HEADERS})

# Link targets
target_link_libraries(MarioGame PRIVATE raylib)
target_include_directories(MarioGame PRIVATE src)




# Set compile flags for Warnings
if(MSVC)
    target_compile_options(MarioGame PRIVATE /W4)
else()
    target_compile_options(MarioGame PRIVATE -Wall -Wextra)
endif()

# Copy asset folders post build
add_custom_command(TARGET MarioGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:MarioGame>/accounts
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:MarioGame>/assets
)
