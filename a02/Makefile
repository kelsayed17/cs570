mush: tokenizer.o main.o command.o
	g++ -std=c++11 tokenizer.o main.o command.o -o mush -lreadline

tokenizer.o: tokenizer.cpp tokenizer.hpp
	g++ -std=c++11 -c tokenizer.cpp

main.o: tokenizer.hpp command.hpp main.cpp
	g++ -std=c++11 -c main.cpp

command.o: command.hpp command.cpp
	g++ -std=c++11 -c command.cpp