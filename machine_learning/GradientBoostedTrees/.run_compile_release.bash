gcc -Wall -g -O3 -march=native -ffast-math -fopenmp src/*.cpp -I include -lstdc++ -std=c++2a
mv a.out bin/release/
bin/release/./a.out
