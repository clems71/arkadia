# arkadia

Arkadia is split in multiple layers:
- **emulation cores** : core-genesis only for the moment
- **frontend** : an *Electron* based HTML5 webapp to play games

## Emulation cores build

C++ emulation cores are built using CMake so you need to install it to be able to build these cores.

### OS X

Dependencies:
- Clang 3.6

To install the genesis core at the right place (next to the frontend), just type the following lines in your console:

        mkdir -p
        cd build
        cmake ../core-genesis -DCMAKE_BUILD_TYPE=Release
        make install -j8


### Windows

**TO BE DONE**


## Frontend build

Dependencies:
- Node with npm (v0.12 at least)

        cd frontend
        npm install && npm run install-fix
        npm start

## Ideas (unordered)

ROM Web service
- coin delay
- description
- screenshots
- name
- console

Electronics USB board for coin system (RFID ?)
