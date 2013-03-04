all:
	g++ -O2 -Wall -o exec main.cpp `pkg-config --cflags --libs opencv`
clean:
	rm -rf exec
