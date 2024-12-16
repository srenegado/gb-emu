make: main.cpp
	g++ main.cpp cpu.cpp graphics.cpp memory.cpp -o main `sdl2-config --cflags --libs`
clean:
	rm -f main