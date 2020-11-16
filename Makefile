SOURCEDIR= source

CXX= g++

CXXFLAGS= -O2 -Wall -I$(SOURCEDIR)

define DEPS
main.o \
Board.o \
ConfigReader.o \
Engine.o \
Interface.o \
Move.o \
PieceInfo.o \
Position.o \
TransTable.o \
Utils.o \
ZobristHash.o
endef

build: $(DEPS)
	$(CXX) $(CXXFLAGS) -o hpp $(DEPS)

main.o:
	$(CXX) $(CXXFLAGS) -c -o main.o main.cpp

Board.o:
	$(CXX) $(CXXFLAGS) -c -o Board.o $(SOURCEDIR)/Board.cpp

ConfigReader.o:
	$(CXX) $(CXXFLAGS) -c -o ConfigReader.o $(SOURCEDIR)/ConfigReader.cpp

Engine.o:
	$(CXX) $(CXXFLAGS) -c -o Engine.o $(SOURCEDIR)/Engine.cpp

Interface.o:
	$(CXX) $(CXXFLAGS) -c -o Interface.o $(SOURCEDIR)/Interface.cpp

Move.o:
	$(CXX) $(CXXFLAGS) -c -o Move.o $(SOURCEDIR)/Move.cpp

PieceInfo.o:
	$(CXX) $(CXXFLAGS) -c -o PieceInfo.o $(SOURCEDIR)/PieceInfo.cpp

Position.o:
	$(CXX) $(CXXFLAGS) -c -o Position.o $(SOURCEDIR)/Position.cpp

TransTable.o:
	$(CXX) $(CXXFLAGS) -c -o TransTable.o $(SOURCEDIR)/TransTable.cpp

Utils.o:
	$(CXX) $(CXXFLAGS) -c -o Utils.o $(SOURCEDIR)/Utils.cpp

ZobristHash.o:
	$(CXX) $(CXXFLAGS) -c -o ZobristHash.o $(SOURCEDIR)/ZobristHash.cpp

clean:
	rm *.o & rm hpp

test:
	@echo $(DEPS)









