## Makefile
.PHONY: run cpu clean

CXX = clang++
CXX_FLAGS = -std=c++17 -Wall -Wextra -Ofast -g -march=native
CPU_SRCS = $(wildcard src/*)
CPU_INCLUDES = -Iinclude/
LIBS = -lstdc++ -fopenmp -lgomp -stdlib=libc++
BIN = bin/release/GBDT

MODULE_NAME = GBDTEngine
PYBIN = bin/python/GBDT/$(MODULE_NAME)
PYBIND11_FLAGS = `python3-config --extension-suffix` python/* -shared -std=c++17 -fPIC `python3 -m pybind11 --includes`

LD_FLAGS = -L/home/jdm365/miniconda3/lib -lpython3.11

cpu:
	$(CXX) $(CXX_FLAGS) -o $(PYBIN)$(PYBIND11_FLAGS) $(CPU_SRCS) $(CPU_INCLUDES) $(LIBS)
	cd bin/python && python -m pip install .

run:
	$(CXX) $(CXX_FLAGS) -o $(BIN) $(CPU_SRCS) $(CPU_INCLUDES) $(LIBS) `python3 -m pybind11 --includes` $(LD_FLAGS)
	./$(BIN)

clean:
	rm -f $(PYBIN).so
	rm -f bin/release/*
	rm -f bin/debug/*
