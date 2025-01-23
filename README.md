# OhBoy - Pico branch

A version of OhBoy to accommodate for the Pi Pico 2. No external dependencies. Pure C++ and compile to a static library

For now, PPU scaling is hardcoded to 320x240 to use with ILI9341 LCD controller. If you intend to use this library with another screen size, do modify `ppu.c` and `ppu.h`! 

## Use with CMake
- Use `add_subdirectory` to add this library to your project
- Use `target_link_library(YOUR_TARGET OhBoy)` to link

## How to build
```
# Run CMake
mkdir build
cd build
cmake ..
cmake --build .
```


## License
    OhBoy is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. 