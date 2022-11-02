g++ -Wall -g -fsanitize=undefined -fopenmp src/*.cpp -I include -std=c++2a
mv a.out bin/debug/
