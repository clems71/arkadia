cmake_minimum_required(VERSION 3.0)
project(retro-api)

include_directories(${CMAKE_JS_INC})

if (APPLE)
  add_definitions(-std=c++1y)
endif()

add_library(${PROJECT_NAME} SHARED core.cpp main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
