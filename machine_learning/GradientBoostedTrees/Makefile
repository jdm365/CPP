## Makefile
.PHONY: cpu clean

CXX = clang++
CXX_FLAGS = -std=c++17 -Wall -Wextra -Ofast -g -march=native

SO_FILE_DIR = ./python
LIBS = -lstdc++ -fopenmp -lgomp -stdlib=libc++

CPU_SRCS = $(wildcard src/*.cpp)

# Paths to include directories
CPU_INCLUDES = -Iinclude

# Linker flags
LDFLAGS = -L/home/jdm365/miniconda3/lib -lpython3.11

MODULE_NAME = GBDTEngine
PYBIN = bin/python/GBDT/$(MODULE_NAME)
PYBIND11_FLAGS = `python3-config --extension-suffix` python/* -shared -std=c++17 -fPIC `python3 -m pybind11 --includes`

#$(CXX) $(CXX_FLAGS) -o $(PYBIN)$(PYBIND11_FLAGS) $(CPU_SRCS) $(CPU_INCLUDES) $(LIBS)
cpu:
	$(CXX) $(CXX_FLAGS) -o $(PYBIN)$(PYBIND11_FLAGS) src/* $(CPU_INCLUDES) $(LIBS)
	cd bin/python && python -m pip install .

clean:
	rm -f $(PYBIN).so
	rm -f bin/release/*
	rm -f bin/debug/*
