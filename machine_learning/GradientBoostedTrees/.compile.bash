case "$(uname -sr)" in

	Linux*Microsoft*)
		g++ -Wall -g -fsanitize=undefined -fopenmp src/*.cpp -I include -std=c++2a;;

	Darwin*)
		gcc -Wall -g -fsanitize=undefined -Xpreprocessor -fopenmp src/*.cpp -I include -L/usr/local/lib -lomp -lstdc++ -std=c++2a;;

esac

mv a.out bin/debug/
