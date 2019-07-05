all: tokenizer

tokenizer: tokenizer.o
	g++ -std=c++11 tokenizer.o -o tokenizer -lreadline

tokenizer.o: tokenizer.cpp
	g++ -std=c++11 -c tokenizer.cpp