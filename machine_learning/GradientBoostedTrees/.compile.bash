g++ -Wall -g -fsanitize=undefined src/*.cpp -I include -std=c++2a
mv a.out bin/debug/
