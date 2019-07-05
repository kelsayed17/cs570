mizzo:main.o Buffer.o Consumer.o Producer.o
	g++ -std=c++11 main.o Buffer.o Consumer.o Producer.o -o mizzo -lpthread -lrt

main.o: main.cpp
	g++ -std=c++11 -c main.cpp

Buffer.o: Buffer.cpp Buffer.h
	g++ -std=c++11 -c Buffer.cpp Buffer.h

Consumer.o: Consumer.cpp Consumer.h
	g++ -std=c++11 -c Consumer.cpp Consumer.h

Producer.o: Producer.cpp Producer.h
	g++ -std=c++11 -c Producer.cpp Producer.h
