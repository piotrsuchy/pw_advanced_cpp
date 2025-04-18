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
