# Path: Makefile
CXX = clang++
CXXFLAGS = -Wall -O3 -std=c++17 -stdlib=libc++
CXXFLAGS += -fopenmp
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf
LDFLAGS += -lSDL2_mixer
LDFLAGS += -lomp
INCLUDES = -I include
BIN = bin/release/game


main:
	$(CXX) $(CXXFLAGS) src/*.cpp $(INCLUDES) $(LDFLAGS) -o $(BIN)

run: main
	./$(BIN)
