## Makefile
.PHONY: run cpu clean

#CXX = clang++
#CXX = gcc
CXX = g++
CXX_FLAGS = -std=c++2a -Wall -Wextra -Ofast -g -march=native
CXX_FLAGS += -fopenmp
CPU_SRCS = $(wildcard src/*)
CPU_INCLUDES = -Iinclude/
ifeq ($(shell uname), Darwin)
	LIBS = -Xpreprocessor -fopenmp -lomp -lstdc++
	LD_FLAGS = -L/Users/jakemehlman/opt/anaconda3/lib -lpython3.8 -Wl,-rpath,/Users/jakemehlman/opt/anaconda3/lib
else
	LIBS = -fopenmp -lstdc++ -lgomp
	LD_FLAGS = -L/home/jdm365/miniconda3/lib -lpython3.11
endif

ifeq ($(CXX), clang++)
	CXX_FLAGS += -stdlib=libc++
endif
BIN = bin/release/GBDT

MODULE_NAME = GBDTEngine
PYBIN = bin/python/GBDT/$(MODULE_NAME)
PYBIND11_FLAGS = `python3-config --extension-suffix` python/* -shared -std=c++2a -fPIC `python3 -m pybind11 --includes`


cpu:
	$(CXX) $(CXX_FLAGS) -o $(PYBIN)$(PYBIND11_FLAGS) $(CPU_SRCS) $(CPU_INCLUDES) $(LIBS) $(LD_FLAGS)
	cd bin/python && python -m pip install .

run:
	$(CXX) $(CXX_FLAGS) -o $(BIN) $(CPU_SRCS) $(CPU_INCLUDES) $(LIBS) `python3 -m pybind11 --includes` $(LD_FLAGS)
	./$(BIN)

clean:
	pip uninstall -y GBDT
	rm -rf $(PYBIN).so
	rm -rf bin/release/*
	rm -rf bin/debug/*
