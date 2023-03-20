# Constants
DEBUG = -g
FLAGS = -march=native -Ofast -flto -D_GBLIBCXX_PARALLEL -fopenmp
CC = g++

# Executable files to ./fdr
fdr: song.h song.cpp chordNote.h chordNote.cpp timestamp.h timestamp.cpp common.h common.cpp main.cpp
	$(CC) $(FLAGS) main.cpp -o fdr
	chmod +x fdr



# Misc
clean:
	rm -f *.o fdr
install:
	cp -f fdr /usr/local/bin
reset:
	make clean && make fdr
uninstall:
	rm /usr/local/bin/fdr
info:
	@echo "*.o: Compiles the binary for the given class"
	@echo "fdr: Compiles the main program"
	@echo "clean: Removes all binary files"
	@echo "install: Installs the program to /usr/local/bin"
	@echo "uninstall: Removes the program from /usr/local/bin"
	@echo "info: Displays this message"



