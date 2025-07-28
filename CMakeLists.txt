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

# Find fmt package
find_package(fmt REQUIRED)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Create list of source files
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# Add tests (optional)
add_executable(${PROJECT_NAME}_test tests/main_test.cpp)
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
