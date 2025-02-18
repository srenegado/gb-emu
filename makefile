make: main.cpp
	g++ main.cpp cpu.cpp cpu_util.cpp cart.cpp memory.cpp instruction_set.cpp interrupt_handler.cpp -o main `sdl2-config --cflags --libs`
debug:
	g++ -g main.cpp cpu.cpp cpu_util.cpp cart.cpp memory.cpp instruction_set.cpp interrupt_handler.cpp -o main `sdl2-config --cflags --libs`
clean:
	rm -f main