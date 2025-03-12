make: main.cpp
	g++ main.cpp cpu.cpp cpu_util.cpp memory.cpp io.cpp instruction_set.cpp interrupt_handler.cpp timer.cpp ppu.cpp event_handler.cpp -o main `sdl2-config --cflags --libs`
debug:
	g++ -g main.cpp cpu.cpp cpu_util.cpp memory.cpp io.cpp instruction_set.cpp interrupt_handler.cpp timer.cpp ppu.cpp event_handler.cpp -o main `sdl2-config --cflags --libs`
clean:
	rm -f main