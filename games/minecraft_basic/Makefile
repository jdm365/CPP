# Variables
CXX = clang++
CXXFLAGS = -O3 -ffast-math -march=native -stdlib=libc++
LDLIBS = -lGLEW -lGLU -lGL -lglfw -lpthread
SRCDIR = src
INCLUDES = -Iinclude
OBJDIR = obj
BINDIR = bin/release
TARGET = game
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Main target
all: $(BINDIR)/$(TARGET)

# Compile and link
$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the game
.PHONY: run
run: $(BINDIR)/$(TARGET)
	./$<

# Clean up
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Alternative compiler: clang
clang:
	make CXX=clang++ CXXFLAGS="$(CXXFLAGS) -stdlib=libc++"

#make: clang
	#clang++ src/* -O3 -ffast-math -march=native -stdlib=libc++ -lglut -lGLU -lGL -o game
	#mv game bin/release/
	#./bin/release/game
