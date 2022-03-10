# CXX Make variable for compiler
CXX=g++
# Make variable for compiler options
#	-std=c++11  C/C++ variant to use, e.g. C++ 2011
#	-g          include information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -g

# output: main.o dictionary.o
# 	g++ main.o dictionary.o -o countwords

# main.o:	main.cpp
# 	g++ -c main.cpp

# dictionary.o: dictionary.cpp dictionary.h
# 	g++ -c dictionary.cpp

# Rules format:
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
page : tracereader.o PageTable.o output_mode_helpers.o main.o 
	$(CXX) $(CXXFLAGS) -o pagingwithtlb $^

main.o: main.cpp

tracereader.o : tracereader.cpp
	$(CXX) $(CXXFLAGS) -c tracereader.cpp

PageTable.o : PageTable.cpp
	$(CXX) $(CXXFLAGS) -c PageTable.cpp

output_mode_helpers.o : output_mode_helpers.cpp

clean :
	rm *.o
