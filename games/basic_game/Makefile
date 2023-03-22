# Path: Makefile
make: src/*.cpp
	clang++ -Wall -O3 src/*.cpp -I include -stdlib=libc++ -lSDL2 -lSDL2_image -o game
	mv game bin/release/
	bin/release/./game
