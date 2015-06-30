# If the RPI target is specified, we should use a cross compiler
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/Toolchain-xc32.cmake)

cmake_minimum_required (VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mprocessor=32MX250F128B -O1 -ffunction-sections")

project(arkadia-pad C)

include_directories(
    src
    sys
    sys/framework
    ${HARMONY_SYSROOT}/framework
)

add_library(harmony 
    ${HARMONY_SYSROOT}/framework/system/devcon/src/sys_devcon.c
    ${HARMONY_SYSROOT}/framework/system/int/src/sys_int_pic32.c
    ${HARMONY_SYSROOT}/framework/driver/usb/usbfs/src/static/drv_usbfs.c
    ${HARMONY_SYSROOT}/framework/system/devcon/src/sys_devcon_pic32mx.c
    ${HARMONY_SYSROOT}/framework/usb/src/dynamic/usb_device_hid.c
    ${HARMONY_SYSROOT}/framework/system/ports/src/sys_ports.c
    ${HARMONY_SYSROOT}/framework/usb/src/dynamic/usb_device.c
    ${HARMONY_SYSROOT}/framework/driver/usb/usbfs/src/static/drv_usbfs_device.c
)

add_executable(arkadia-pad
    src/main.c
    src/app.c
    sys/system_tasks.c
    sys/system_interrupt.c
    sys/system_init.c
    sys/system_exceptions.c
    sys/framework/system/clk/src/sys_clk_static.c
    sys/framework/system/ports/src/sys_ports_static.c
)

# add_definitions(-Wl,--defsym=__MPLAB_BUILD=1,--defsym=_min_heap_size=0,--gc-sections)
target_link_libraries(arkadia-pad 
    ${HARMONY_SYSROOT}/bin/framework/peripheral/PIC32MX250F128B_peripherals.a 
    harmony
    "-Wl,--gc-sections,--defsym=_min_heap_size=0"
)
