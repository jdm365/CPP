case "$(uname -sr)" in

	Linux*Microsoft*)
		gcc -Wall -g -O3 -march=native -ffast-math -fopenmp src/*.cpp -I include -std=c++2a;;

	Darwin*)
		g++ -Wall -g -Xpreprocessor -fopenmp src/*.cpp -I include -L/usr/local/lib -lomp -lstdc++ -std=c++2a;;

esac

mv a.out bin/debug/
./bin/debug/a.out
