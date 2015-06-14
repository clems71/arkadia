# arkadia

## Build

### Windows

Currently, only MSYS2 mingw64 is tested and supported on this platform.

Dependencies:
- SDL2
- GCC 4.9.2+

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles"
    make -j8

## Ideas (unordered)

ROM Web service
- coin delay
- description
- screenshots
- name
- console

Electronics USB board for joysticks (inputs)
- build a mips32 gcc for PIC32
- buy joysticks, buttons, etc...

Electronics USB board for coin system (RFID ?)

Try to build the SDL2 sample for Odroid C1
- maybe switch to straight opengl
- buildroot - custom fbdev only - no acceleration

Frontend polishing (C++ / Qt / HTML / JS / Kivy ?)
