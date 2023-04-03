## Makefile

CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++ -Wall -Wextra -Werror -pedantic -g
CXXFLAGS += -mtune=native -march=native -O3 -fopenmp
LDLIBS = -lstdc++ -lomp
SRCDIR = src
INCLUDES = -Iinclude
OBJDIR = obj
BINDIR = bin/release
TARGET = $(BINDIR)/main
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

## Main target
all: $(TARGET)

## Linking
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

## Compiling
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

## Cleaning
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

## Running
.PHONY: run
run: $(TARGET)
	$(TARGET)
