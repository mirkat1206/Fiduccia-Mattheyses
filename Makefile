CC = g++
CFLAGS = -std=c++11 -O3 -lm
EXECUTABLE = bin/fm

all: $(EXECUTABLE)

$(EXECUTABLE): src/main.cpp fm.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) src/main.cpp fm.o

fm.o: src/fm.cpp src/fm.h src/cell.h src/net.h src/bl.h
	$(CC) $(CFLAGS) -o fm.o -c src/fm.cpp

clean:
	rm -rf *.o $(EXECUTABLE)
