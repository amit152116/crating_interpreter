cmake_minimum_required(VERSION 3.16)
project(
  Krypton
  VERSION 1.0.0
  LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Options
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_WARNINGS "Enable compiler warnings" OFF)

# Compiler warnings
if(ENABLE_WARNINGS)
  if(MSVC)
    add_compile_options(/W4 /WX)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      add_compile_options(-Werror)
    endif()
  endif()
endif()

# Dependencies
find_package(fmt REQUIRED)

# Add shared library
add_subdirectory(lib)

# Source files (library)
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Executable (only main.cpp or app entry points)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Krypton_lib)
target_include_directories(${PROJECT_NAME}
                           PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Install
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Tests
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
