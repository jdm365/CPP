case "$(uname -sr)" in

	Linux*)
		g++ -Wall -g -O3 -march=native -ffast-math -fopenmp src/*.cpp -I include -std=c++2a;;

	Darwin*)
		gcc -Wall -g -O3 -march=native -ffast-math -Xpreprocessor -fopenmp src/*.cpp -I include -L/usr/local/lib -lomp -lstdc++ -std=c++2a;;
		
esac

mv a.out bin/release/
bin/release/./a.out
