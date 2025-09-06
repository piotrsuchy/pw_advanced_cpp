cmake_minimum_required(VERSION 3.14)
project(PacManGame)

set(CMAKE_CXX_STANDARD 17)

option(ENABLE_SINGLEPLAYER "Build legacy singleplayer target" ON)

# Include paths
include_directories(include)

# Add SFML
find_package(SFML 2.5 COMPONENTS graphics window system network REQUIRED)

# Source files (legacy singleplayer)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/core/*.cpp
    src/graphics/*.cpp
    main.cpp
)

# Add shared logic library (graphics-free)
file(GLOB_RECURSE SHARED_LOGIC CONFIGURE_DEPENDS
    src/shared/*.cpp
)
add_library(pacman_shared STATIC ${SHARED_LOGIC})

if(ENABLE_SINGLEPLAYER)
  # Define the executable
  add_executable(PacManGame ${SOURCES})
  # Link SFML libraries and shared logic
  target_link_libraries(PacManGame sfml-graphics sfml-window sfml-system pacman_shared)
endif()

# -----------------------------------------------------------------------------
# Server / Client targets for multiplayer PoC
# -----------------------------------------------------------------------------

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
    src/server/*.cpp
)

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
    src/client/*.cpp
)

if(SERVER_SOURCES)
  add_executable(pacman_server ${SERVER_SOURCES})
  target_link_libraries(pacman_server pacman_shared sfml-network sfml-system)
  # Logic-only dependency for level grid
  target_sources(pacman_server PRIVATE src/core/LevelManager.cpp)
endif()

if(CLIENT_SOURCES)
  add_executable(pacman_client ${CLIENT_SOURCES})
  target_link_libraries(pacman_client pacman_shared sfml-graphics sfml-window sfml-network sfml-system)
  # Reuse core/graphics modules needed for rendering and input
  target_sources(pacman_client PRIVATE
    src/core/InputManager.cpp
    src/core/LevelManager.cpp
    src/graphics/LevelRenderer.cpp
    src/graphics/PacmanRenderer.cpp
  )
endif()

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
