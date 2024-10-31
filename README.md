# OhBoy (WIP)

An (hopefully) M-Cycle accurate Game Boy emulator. I will release pre-built binary after
rom selection

## Images



## TODO

- Debugger:
  - Add ability to reset CPU state
  - Allow toggling file logging
  - Add breakpoint by PC instead of current log line
  - Allow selecting ROM
  - Turn off debugging windows in Release mode
- CPU:
  - Implement STOP (or not)
- Cartridge: 
  - Saving SRAM to a file
  - Implement MBC3 support
  - Move VRAM to Memory
- PPU:
  - Implement Mode 3 scroll penalty
- Implement APU

## Known Issues

- PPU:
  - Sprite glitching in Zelda Link's Awakening intro
  - Object priority is sometimes wrong
- Cartridge:
  - Only support MBC1 and MBC5 games (for now)

## License
GPLv3