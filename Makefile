all:
	g++ -O2 -Wall -o exemple ex_opencv.cpp `pkg-config --cflags --libs opencv`
clean:
	rm -rf exemple
