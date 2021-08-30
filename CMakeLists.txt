cmake_minimum_required(VERSION 3.10)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(NES VERSION 1.0)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(nes ${SRC_FILES})

# Libraries

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(nes ${SDL2_LIBRARIES})

# Header Files
target_include_directories(nes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/2C02)
target_include_directories(nes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/6502)
target_include_directories(nes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/cartridge)
target_include_directories(nes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/mapper)