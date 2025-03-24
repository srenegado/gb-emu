# gb-emu

A Game Boy emulator written in C++ with SDL2. Tested on an Ubuntu system.

The goals of this project are to have a challenging learning experience, gain a deeper appreciation with how consoles and computers work, and because I have to get *Pokémon Red* to run.

### a slight preamble

I wanted to challenge myself with a project I thought was pretty complicated and difficult, but also tangible and knew with enough effort, I could finish. Having said that, I had no clue where to start. So, I started to search around for some ideas and stumbled on this [blog post](https://austinhenley.com/blog/challengingprojects.html). I found that an emulator was a perfect fit: I'm a big fan of the Game Boy Advance (perhaps a project for another time), and the Game Boy was a solid starting point for budding emulator devs from what I gathered on `r/EmuDev`.

## Usage
Right now, the emulator is only supported on Linux, and you also have to build and make the binary, so you'll need `make` and `gcc`. Build the emulator with the `make` file. 

```
./main [options] path/to/rom

options:
-s path/to/sav  Load an existing or create a new .sav file for games that support it.
                        
```

## Tests
Passed:
- Blargg's `cpu_instrs` and `instr_timing` tests

The emulator fails the "Window internal line counter" test case of the `dmg-acid2` test, but passes everything else.

## Resources
There are a ton of useful resources and other reference emulators freely available. Here are some useful ones I used:
- [Pandocs](https://gbdev.io/pandocs/)
- [GBEDG](https://hacktix.github.io/GBEDG/)
- [About swotGB](https://mitxela.com/projects/swotgb/about)
- [Gameboy Emulator Development Series by Low Level Devel](https://www.youtube.com/watch?v=e87qKixKFME&list=PLVxiWMqQvhg_yk4qy2cSC3457wZJga_e5&ab_channel=LowLevelDevel)
- [The Ultimate Game Boy Talk](https://www.youtube.com/watch?v=HyzD8pNlpwI&ab_channel=media.ccc.de) 
