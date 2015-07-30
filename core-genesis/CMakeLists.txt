cmake_minimum_required(VERSION 3.0)
project(core-genesis)

include(GenerateExportHeader)

add_definitions(-DLSB_FIRST -DUSE_32BPP_RENDERING)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/../frontend/cores)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

add_library(core-genesis SHARED
  src/genplusgx_source/cart_hw/areplay.c
  src/genplusgx_source/cart_hw/eeprom_93c.c
  src/genplusgx_source/cart_hw/eeprom_i2c.c
  src/genplusgx_source/cart_hw/eeprom_spi.c
  src/genplusgx_source/cart_hw/ggenie.c
  src/genplusgx_source/cart_hw/md_cart.c
  src/genplusgx_source/cart_hw/sms_cart.c
  src/genplusgx_source/cart_hw/sram.c
  src/genplusgx_source/cart_hw/svp/ssp16.c
  src/genplusgx_source/cart_hw/svp/svp.c
  src/genplusgx_source/cd_hw/cd_cart.c
  src/genplusgx_source/cd_hw/cdc.c
  src/genplusgx_source/cd_hw/cdd.c
  src/genplusgx_source/cd_hw/gfx.c
  src/genplusgx_source/cd_hw/pcm.c
  src/genplusgx_source/cd_hw/scd.c
  src/genplusgx_source/genesis.c
  src/genplusgx_source/input_hw/activator.c
  src/genplusgx_source/input_hw/gamepad.c
  src/genplusgx_source/input_hw/input.c
  src/genplusgx_source/input_hw/lightgun.c
  src/genplusgx_source/input_hw/mouse.c
  src/genplusgx_source/input_hw/paddle.c
  src/genplusgx_source/input_hw/sportspad.c
  src/genplusgx_source/input_hw/teamplayer.c
  src/genplusgx_source/input_hw/terebi_oekaki.c
  src/genplusgx_source/input_hw/xe_a1p.c
  src/genplusgx_source/io_ctrl.c
  src/genplusgx_source/loadrom.c
  src/genplusgx_source/m68k/m68kcpu.c
  src/genplusgx_source/m68k/s68kcpu.c
  src/genplusgx_source/mem68k.c
  src/genplusgx_source/membnk.c
  src/genplusgx_source/memz80.c
  src/genplusgx_source/ntsc/md_ntsc.c
  src/genplusgx_source/ntsc/sms_ntsc.c
  src/genplusgx_source/sound/blip_buf.c
  src/genplusgx_source/sound/eq.c
  src/genplusgx_source/sound/sn76489.c
  src/genplusgx_source/sound/sound.c
  src/genplusgx_source/sound/ym2413.c
  src/genplusgx_source/sound/ym2612.c
  src/genplusgx_source/state.c
  src/genplusgx_source/system.c
  src/genplusgx_source/tremor/bitwise.c
  src/genplusgx_source/tremor/block.c
  src/genplusgx_source/tremor/codebook.c
  src/genplusgx_source/tremor/floor0.c
  src/genplusgx_source/tremor/floor1.c
  src/genplusgx_source/tremor/framing.c
  src/genplusgx_source/tremor/info.c
  src/genplusgx_source/tremor/mapping0.c
  src/genplusgx_source/tremor/mdct.c
  src/genplusgx_source/tremor/registry.c
  src/genplusgx_source/tremor/res012.c
  src/genplusgx_source/tremor/sharedbook.c
  src/genplusgx_source/tremor/synthesis.c
  src/genplusgx_source/tremor/vorbisfile.c
  src/genplusgx_source/tremor/window.c
  src/genplusgx_source/vdp_ctrl.c
  src/genplusgx_source/vdp_render.c
  src/genplusgx_source/z80/z80.c
  src/scrc32.c

  src/core.cpp
)

target_include_directories(core-genesis PRIVATE
  src
  src/genplusgx_source
  src/genplusgx_source/cart_hw
  src/genplusgx_source/cart_hw/svp
  src/genplusgx_source/cd_hw
  src/genplusgx_source/input_hw
  src/genplusgx_source/m68k
  src/genplusgx_source/ntsc
  src/genplusgx_source/sound
  src/genplusgx_source/tremor
  src/genplusgx_source/z80
)

target_include_directories(core-genesis PUBLIC
  include
  ${CMAKE_CURRENT_BINARY_DIR}
)

# Create shared library export helper header
generate_export_header(core-genesis BASE_NAME CORE)

install(FILES
 include/core.h
 ${PROJECT_BINARY_DIR}/core_export.h
 DESTINATION include
)

install(TARGETS core-genesis
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
