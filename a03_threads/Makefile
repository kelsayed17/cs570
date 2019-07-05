d_demo: thread_demo.o
	g++ -g -std=c++11 thread_demo.o worker.o -o thread_demo -lpthread

thread_demo.o: thread_demo.cpp
	g++ -g -std=c++11 -c thread_demo.cpp worker.cpp worker.hpp
