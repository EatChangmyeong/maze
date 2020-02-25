.PHONY: clean

CC = g++
OBJFILES = maze.o maze_algorithms.o

all: maze.exe

maze.exe: $(OBJFILES)
	$(CC) $(OBJFILES) -o $@

maze.o: maze.cpp
	$(CC) $? -c -o $@

maze_algorithms.o: maze_algorithms.cpp
	$(CC) $? -c -o $@

clean:
	del $(OBJFILES) maze.exe
