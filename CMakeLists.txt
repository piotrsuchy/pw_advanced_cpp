cmake_minimum_required(VERSION 3.14)
project(PacManGame)

set(CMAKE_CXX_STANDARD 17)

# Include paths
include_directories(include)

# Add SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/core/*.cpp
    src/graphics/*.cpp
    main.cpp
)

# Define the executable
add_executable(PacManGame ${SOURCES})

# Link SFML libraries
target_link_libraries(PacManGame sfml-graphics sfml-window sfml-system)

# -----------------------------------------------------------------------------
# Developer convenience targets: clang-format and clang-tidy
# -----------------------------------------------------------------------------
find_program(CLANG_FORMAT_EXE NAMES clang-format)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/main.cpp
)

if(CLANG_FORMAT_EXE)
  add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_CXX_SOURCE_FILES}
    COMMENT "Running clang-format")

  add_custom_target(format-check
    COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_CXX_SOURCE_FILES}
    COMMENT "Checking clang-format")
endif()

if(CLANG_TIDY_EXE)
  # Allow using: cmake -DCMAKE_CXX_CLANG_TIDY=clang-tidy ..
  set(CMAKE_CXX_CLANG_TIDY ${CMAKE_CXX_CLANG_TIDY} CACHE STRING "Clang-Tidy executable")

  add_custom_target(tidy
    COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${ALL_CXX_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy")
endif()
