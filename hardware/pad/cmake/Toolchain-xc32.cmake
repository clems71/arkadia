SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

# where is the target environment 
# SET(CMAKE_FIND_ROOT_PATH "C:/Program Files (x86)/Microchip/xc32/v1.34")

# Set path to compilers
SET(CMAKE_C_COMPILER xc32-gcc)
SET(HARMONY_SYSROOT "C:/microchip/harmony/v1_05")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
