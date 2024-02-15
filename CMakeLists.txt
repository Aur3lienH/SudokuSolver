cmake_minimum_required(VERSION 3.10)
project(Solver VERSION 1.0)

# Set the c compiler
set(CMAKE_C_COMPILER gcc)

# c standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add -03 and -ffast-math flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -ffast-math")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ")

# Include directories
include_directories(include)

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)
pkg_check_modules(CAIRO REQUIRED cairo)

# Link libraries
link_libraries(${GTK3_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${CAIRO_LIBRARIES} m SDL2)

# Add the executable
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})

# Specify include directories for the target
target_include_directories(${PROJECT_NAME} PRIVATE ${GTK3_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${CAIRO_LIBRARIES} m SDL2)

# Custom target for cleaning the build directory
add_custom_target(clean-all
   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/CleanAll.cmake
)


