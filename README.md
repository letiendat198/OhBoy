# OhBoyPico

Adapt OhBoy to Pi Pico 2

## How to build
#### This is a guide for MinGW only! MSVC is probably the same.
- You will need to install [Rust](https://www.rust-lang.org/tools/install) and MinGW
- Download pre-built [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.8) (the devel-mingw variant), 
rename it SDL2 then put it inside `deps` folder
```
# Clone this repository and its dependencies
git clone --recurse-submodules https://github.com/letiendat198/OhBoy.git

# My Rust setup uses MSVC by default so I need to target it to build for MinGW
# If your Rust setup is using MinGW already then skip this
rustup target add --toolchain stable-x86_64-pc-windows-msvc x86_64-pc-windows-gnu

# Run CMake
mkdir build
cd build
cmake .. -G 'MinGW Makefiles'
cmake --build .
```

## TODO

- Re-write APU
- Re-do RTC
- Debugger:
  - Add ability to reset CPU state
  - Allow toggling file logging

## Known Issues

- PPU:
  - LY = 153 oddities bug

## License
    OhBoy is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. 