# gb-emu

A Game Boy emulator written in C++ with SDL2. Tested on an Ubuntu system.

The goals of this project are to have a challenging learning experience, gain a deeper appreciation with how consoles and computers work, and because I wanted to get *Pokémon Red* to run.

<p align="center">
  <img src="https://github.com/user-attachments/assets/1a7fc328-fa53-4554-8200-5e895e845326" width="425">
</p>

### a slight preamble

I wanted to challenge myself with a project I thought was pretty complicated and difficult, but was also tangible and knew with enough effort, I could finish it. I had no clue where to start, though. So I started to search around for some ideas and after stumbling on a [blog post](https://austinhenley.com/blog/challengingprojects.html), I found that an emulator was a perfect fit: I'm a big fan of the Game Boy Advance (perhaps a project for another time), and the original Game Boy is a solid starting point for beginner emulator devs from what I saw on `r/EmuDev`.

## Usage
Right now, the emulator is only supported on Linux and you also have to build and make it. For that you'll need [SDL2 installed](https://wiki.libsdl.org/SDL2/Installation), `make`, and `g++`. To build `gb-emu`, run `make`.

Then you can run games with the following command:

```
./gb-emu [options] path/to/rom

options:
-s path/to/sav  Load an existing or create a new .sav file for games that support it.
```

## Tests
Passed:
- Blargg's `cpu_instrs` and `instr_timing` tests

The emulator fails the "Window internal line counter" test case of the `dmg-acid2` test, but passes everything else.

## Resources
There are a ton of useful resources and other reference emulators available. Here are some that I used:
- [Pandocs](https://gbdev.io/pandocs/)
- [GBEDG](https://hacktix.github.io/GBEDG/)
- [About swotGB](https://mitxela.com/projects/swotgb/about)
- [Gameboy Emulator Development Series by Low Level Devel](https://www.youtube.com/watch?v=e87qKixKFME&list=PLVxiWMqQvhg_yk4qy2cSC3457wZJga_e5&ab_channel=LowLevelDevel)
- [The Ultimate Game Boy Talk](https://www.youtube.com/watch?v=HyzD8pNlpwI&ab_channel=media.ccc.de) 
