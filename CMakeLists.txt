cmake_minimum_required(VERSION 3.14)
project(M811RGB C)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-g -Wall)

# ============================================================
# Language standard required by Clay
# ============================================================
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


# ============================================================
# Raylib (built from vendor/)
# ============================================================
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/raylib)


# ============================================================
# System libusb
# ============================================================
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)


# ============================================================
# Include paths
# ============================================================
include_directories(
    include
    vendor/clay
    vendor/raylib/src
    ${LIBUSB_INCLUDE_DIRS}
)


# ============================================================
# Collect C sources
# ============================================================
file(GLOB PROJECT_SOURCES
    src/*.c
)


# ============================================================
# Build the executable
# ============================================================
add_executable(M811-RGB ${PROJECT_SOURCES})


# ============================================================
# Link libraries
# ============================================================
target_link_libraries(M811-RGB
    raylib
    ${LIBUSB_LIBRARIES}
    m
    pthread
    dl
)


# ============================================================
# Warning Flags
# ============================================================
target_compile_options(M811-RGB PRIVATE
    -Wall
    -Wextra
    -Werror
)