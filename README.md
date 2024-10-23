# OhBoy (WIP)

A (hopefully) M-Cycle accurate GameBoy emulator

## TODO

- Fix CPU to pass tests (Pass all of Blargg's tests except 02-interrupts)
- Debugger:
  - Add ability to reset CPU state
  - Allow closing log file
  - Allow toggling file logging
  - Add breakpoint by PC instead of current log line
  - Finish serial
- Implement interrupts
- Implement OAM DMA
- Implement PPU
- Implement Joystick
- Cartridge: 
  - Saving SRAM to a file
  - Implement MBC1,2,3,5 support

## Fixed issues

- Serial output garbage: Caused by not resetting bit 7 of 0xFF02 (serial control)
after read out
- Debug console is buggy or sometimes outputs pure garbage: ImVector do something funny with std::string.
Use stdlib stuffs work just fine
- Using std::string as a buffer for file writing is a bad idea. Will overflow. Use stringstream instead.

## License
GPLv3