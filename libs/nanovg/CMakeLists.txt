add_library(nanovg src/nanovg.c)

target_include_directories(nanovg PUBLIC include)
target_link_libraries(nanovg stb)
