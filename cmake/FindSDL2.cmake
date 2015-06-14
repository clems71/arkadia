#   SDL2_LIBRARY, the name of the library to link against
#   SDL2_FOUND, if false, do not try to link to SDL
#   SDL2_INCLUDE_DIR, where to find SDL.h
#   SDL2_VERSION_STRING, human-readable string containing the version of SDL

find_path(SDL_INCLUDE_DIR SDL.h
  HINTS
  ENV SDL2DIR
  PATH_SUFFIXES SDL2
  # path suffixes to search inside ENV{SDLDIR}
  include/SDL2 include
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VC_LIB_PATH_SUFFIX lib/x64)
else()
  set(VC_LIB_PATH_SUFFIX lib/x86)
endif()

find_library(SDL_LIBRARY_TEMP
  NAMES SDL2
  HINTS
  ENV SDL2DIR
  PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX}
)

if(SDL_LIBRARY_TEMP)
  # For SDLmain
  if(SDLMAIN_LIBRARY AND NOT SDL_BUILDING_LIBRARY)
    list(FIND SDL_LIBRARY_TEMP "${SDLMAIN_LIBRARY}" _SDL_MAIN_INDEX)
    if(_SDL_MAIN_INDEX EQUAL -1)
      set(SDL_LIBRARY_TEMP "${SDLMAIN_LIBRARY}" ${SDL_LIBRARY_TEMP})
    endif()
    unset(_SDL_MAIN_INDEX)
  endif()

  # For OS X, SDL uses Cocoa as a backend so it must link to Cocoa.
  # CMake doesn't display the -framework Cocoa string in the UI even
  # though it actually is there if I modify a pre-used variable.
  # I think it has something to do with the CACHE STRING.
  # So I use a temporary variable until the end so I can set the
  # "real" variable in one-shot.
  if(APPLE)
    set(SDL_LIBRARY_TEMP ${SDL_LIBRARY_TEMP} "-framework Cocoa")
  endif()

  # For threads, as mentioned Apple doesn't need this.
  # In fact, there seems to be a problem if I used the Threads package
  # and try using this line, so I'm just skipping it entirely for OS X.
  if(NOT APPLE)
    set(SDL_LIBRARY_TEMP ${SDL_LIBRARY_TEMP} ${CMAKE_THREAD_LIBS_INIT})
  endif()

  # For MinGW library
  if(MINGW)
    set(SDL_LIBRARY_TEMP ${MINGW32_LIBRARY} ${SDL_LIBRARY_TEMP})
  endif()

  # Set the final string here so the GUI reflects the final state.
  set(SDL_LIBRARY ${SDL_LIBRARY_TEMP} CACHE STRING "Where the SDL Library can be found")
  # Set the temp variable to INTERNAL so it is not seen in the CMake GUI
  set(SDL_LIBRARY_TEMP "${SDL_LIBRARY_TEMP}" CACHE INTERNAL "")
endif()

# if(SDL_INCLUDE_DIR AND EXISTS "${SDL_INCLUDE_DIR}/SDL_version.h")
#   file(STRINGS "${SDL_INCLUDE_DIR}/SDL_version.h" SDL_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+[0-9]+$")
#   file(STRINGS "${SDL_INCLUDE_DIR}/SDL_version.h" SDL_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL_MINOR_VERSION[ \t]+[0-9]+$")
#   file(STRINGS "${SDL_INCLUDE_DIR}/SDL_version.h" SDL_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL_PATCHLEVEL[ \t]+[0-9]+$")
#   string(REGEX REPLACE "^#define[ \t]+SDL_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL_VERSION_MAJOR "${SDL_VERSION_MAJOR_LINE}")
#   string(REGEX REPLACE "^#define[ \t]+SDL_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL_VERSION_MINOR "${SDL_VERSION_MINOR_LINE}")
#   string(REGEX REPLACE "^#define[ \t]+SDL_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL_VERSION_PATCH "${SDL_VERSION_PATCH_LINE}")
#   set(SDL_VERSION_STRING ${SDL_VERSION_MAJOR}.${SDL_VERSION_MINOR}.${SDL_VERSION_PATCH})
#   unset(SDL_VERSION_MAJOR_LINE)
#   unset(SDL_VERSION_MINOR_LINE)
#   unset(SDL_VERSION_PATCH_LINE)
#   unset(SDL_VERSION_MAJOR)
#   unset(SDL_VERSION_MINOR)
#   unset(SDL_VERSION_PATCH)
# endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(SDL2 REQUIRED_VARS SDL_LIBRARY SDL_INCLUDE_DIR)
