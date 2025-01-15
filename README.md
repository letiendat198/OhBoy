# OhBoy - Pico branch

A version of OhBoy without debugger and logger with no external dependencies. Pure C++ and compile
to a static library

## How to build
```
# Run CMake
mkdir build
cd build
cmake ..
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