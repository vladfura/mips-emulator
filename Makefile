all: project

project: main.o src/core.o src/memory.o src/tlb.o src/executor.o
	g++ main.o src/core.o src/memory.o src/tlb.o src/executor.o -o main
main.o: main.cpp
	g++ -c main.cpp
	
core.o: src/core.cpp
	g++ -c src/core.cpp
	
memory.o: src/memory.cpp
	g++ -c src/memory.cpp
	
tlb.o: src/tlb.cpp
	g++ -c src/tlb.cpp
	
executor.o: src/executor.cpp	
	g++ -c src/executor.cpp	
	
.PHONY:clean
clean:
	rm -rf src/*.o main.o main


