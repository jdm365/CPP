g++ -Wall -g -Ofast -funroll-loops -march=native src/*.cpp -I include -std=c++2a
mv a.out bin/release/
bin/release/./a.out
