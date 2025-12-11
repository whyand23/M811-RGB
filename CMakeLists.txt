cmake_minimum_required(VERSION 3.27)
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
#set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
#add_subdirectory(vendor/raylib)

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "5.5"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(raylib)

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
    vendor/raylib
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
# Warning Flags / cusotom commands
# ============================================================
target_compile_options(M811-RGB PRIVATE
    -Wall
    -Werror
)

target_include_directories(M811-RGB PUBLIC .)

#for copying resource file to build folder
# Copy resources (images, fonts, etc.) into the output directory
add_custom_command(TARGET M811-RGB POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:M811-RGB>/resources

) # For exporting compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
