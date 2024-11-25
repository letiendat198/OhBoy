# OhBoy

An M-Cycle accurate Game Boy (WIP Color) emulator

The emulator core was written in C/C++ and the GUI was written in Rust (because C++ GUI libraries
are either big or ugly)

## Images

![](images/image1.png)
![](images/image2.png)
![](images/image3.png)
![](images/image4.png)
![](images/image5.png)
![](images/image6.png)

## Keyboard Mapping
```
Game Boy:

       |Up|          |Select| |Start| 
|Left||Down||Right|                     |B| |A|

Keyboard:

   |W|          |T| |Y| 
|A||S||D|                     |N| |M|
      
```

## How to build
#### This is a guide for MinGW only! MSVC is probably the same.
- You will need to install [Rust](https://www.rust-lang.org/tools/install) and MinGW
- Download pre-built [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.8) (the devel-mingw variant), 
rename it SDL2 then put it inside `deps` folder
```
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

- CPU:
  - Implement STOP (or not)
- PPU:
  - Implement Mode 3 scroll penalty
- Implement APU
- Debugger:
  - Add ability to reset CPU state
  - Allow toggling file logging
  - Add breakpoint by PC instead of current log line

## Known Issues

- PPU:
  - LY = 153 oddities bug

## License
GPLv3