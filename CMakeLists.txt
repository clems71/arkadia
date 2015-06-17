cmake_minimum_required(VERSION 2.8)
project(arkadia)

include(CheckIncludeFiles)

check_include_files(GLES2/gl2.h HAVE_OPENGLES2)
check_include_files(OpenGL/gl.h HAVE_OPENGL_FRAMEWORK)
check_include_files(GL/gl.h HAVE_OPENGL)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Add extra search path for find_package
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Compiler flags - We use C++14 !!!
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wextra")

find_package(SDL2 REQUIRED)

add_subdirectory(libs/stb)
add_subdirectory(libs/nanovg)

# Include emulation cores here
add_subdirectory(genesis)

add_executable(arkadia 
  font_speedwagon.cpp
  main.cpp
  oglprogram.cpp
  ogltexture.cpp
)

target_include_directories(arkadia PRIVATE ${SDL_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if (MINGW)
  find_package(GLEW REQUIRED)
  target_link_libraries(arkadia ${GLEW_LIBRARIES} mingw32)
endif ()

if (${HAVE_OPENGLES2}) # Embedded case - OpenGL ES 2.0
  target_link_libraries(arkadia EGL)
else() # Desktop case - OpenGL
  find_package(OpenGL REQUIRED)
  target_link_libraries(arkadia ${OPENGL_LIBRARIES})
endif()

target_link_libraries(arkadia ${SDL_LIBRARY} SDL2main genesis nanovg)
