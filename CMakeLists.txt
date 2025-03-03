cmake_minimum_required(VERSION 3.10)
project(PacmanGame VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set SDK path
execute_process(
    COMMAND xcrun --show-sdk-path
    OUTPUT_VARIABLE MAC_SDK_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set SFML paths
set(SFML_DIR "/opt/homebrew/opt/sfml@2/lib/cmake/SFML")

# Find required packages
find_package(SFML 2.6 COMPONENTS graphics window system network REQUIRED)

# Add source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set include directories with correct order
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1
    /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include
    ${SFML_INCLUDE_DIR}
    /opt/homebrew/opt/sfml@2/include
)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -nostdinc++ \
    -isystem /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1")

# Link directories
target_link_directories(${PROJECT_NAME} PRIVATE
    /opt/homebrew/opt/sfml@2/lib
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-network
    c++
) 