g++ -Wall -g -fopenmp src/*.cpp -I include -std=c++2a
mv a.out bin/debug/
./bin/debug/a.out
