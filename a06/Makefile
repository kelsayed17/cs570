CXX = g++  # C++ compiler
CXXFLAGS = -g -c  # C++ flags

# Replace test_suite.C with your code

disksim : BlockDevice.o test_suite.o hexdump.o
	$(CXX) -o disksim -g BlockDevice.o test_suite.o hexdump.o 

BlockDevice.o : BlockDevice.C BlockDevice.h

hexdump.o : hexdump.C hexdump.h

test_suite.o : test_suite.C BlockDevice.h

clean :
	rm *.o
