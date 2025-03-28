CXX = g++
CXXFLAGS = -Wall
SDL2 = `sdl2-config --cflags --libs`

all: gb-emu

gb-emu: main.o cpu.o cpu_util.o memory.o io.o instruction_set.o interrupt_handler.o timer.o ppu.o event_handler.o joypad.o
	${CXX} ${CXXFLAGS} $^ -o $@ ${SDL2}

main.o: main.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

cpu.o: cpu.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

cpu_util.o: cpu_util.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

memory.o: memory.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}
	
io.o: io.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

instruction_set.o: instruction_set.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

interrupt_handler.o: interrupt_handler.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

timer.o: timer.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

ppu.o: ppu.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

event_handler.o: event_handler.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

joypad.o: joypad.cpp
	${CXX} ${CXXFLAGS} -c $^ -o $@ ${SDL2}

clean:
	rm -f gb-emu *.o