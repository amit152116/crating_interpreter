cmake_minimum_required(VERSION 3.10)
project(Krypton)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ensure build files go to build directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Set compiler flags
set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
# add_compile_options(-Wall -Wextra -Wpedantic)

# Find fmt package
find_package(fmt REQUIRED)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Create list of source files
file(GLOB SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Separate main.cpp from other sources
list(REMOVE_ITEM SRC_FILES "${PROJECT_SOURCE_DIR}/src/main.cpp")

# App executable
add_executable(${PROJECT_NAME} ${SRC_FILES} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# Test executable (without main.cpp)
add_executable(${PROJECT_NAME}_test tests/main_test.cpp ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}_test PRIVATE fmt::fmt)

# Create symbolic link to executable in project root (Unix-like systems only)
if(UNIX)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
      ${CMAKE_SOURCE_DIR}/${PROJECT_NAME})

  # Symlink for compile_commands.json
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_BINARY_DIR}/compile_commands.json
      ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif()
