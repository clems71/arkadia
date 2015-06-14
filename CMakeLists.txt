cmake_minimum_required(VERSION 2.8)
project(arkadia)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Compiler flags - We use C++14 !!!
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

find_package(SDL2 REQUIRED)

# Include emulation cores here
add_subdirectory(genesis)

include_directories(
  ${SDL_INCLUDE_DIR}
  $<TARGET_PROPERTY:genesis,INCLUDE_DIRECTORIES>
)

add_executable(arkadia main.cpp)

if (MINGW)
  target_link_libraries(arkadia mingw32)
endif ()

target_link_libraries(arkadia ${SDL_LIBRARY} SDL2main genesis)

