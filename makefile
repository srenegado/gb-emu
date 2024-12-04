make: main.cpp
	g++ main.cpp -o main `sdl2-config --cflags --libs`
clean:
	rm -f main