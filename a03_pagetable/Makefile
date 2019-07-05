pagetable: pagetable.o level.o byu_tracereader.o main.o
	g++ -std=c++11 pagetable.o level.o byu_tracereader.o main.o -o pagetable

pagetable.o: pagetable.h pagetable.cpp
	g++ -std=c++11 -c pagetable.cpp

level.o: level.h level.cpp
	g++ -std=c++11 -c level.cpp

byu_tracereader.o: byutr.h byu_tracereader.c
	gcc -c byu_tracereader.c

main.o: pagetable.h level.h map.h main.cpp
	g++ -std=c++11 -c main.cpp
