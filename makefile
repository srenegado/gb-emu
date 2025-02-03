make: main.cpp
	g++ main.cpp cpu.cpp -o main `sdl2-config --cflags --libs`
clean:
	rm -f main