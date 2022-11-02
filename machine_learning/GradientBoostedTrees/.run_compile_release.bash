g++ -Wall -g -Ofast -march=native -funroll-loops -fopenmp src/*.cpp -I include -std=c++2a
mv a.out bin/release/
bin/release/./a.out
