make: main.cpp
	g++ main.cpp cpu.cpp cpu_util.cpp cart.cpp memory.cpp -o main `sdl2-config --cflags --libs`
clean:
	rm -f main