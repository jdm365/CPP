case "$(uname -sr)" in

	Linux*)
		g++ -Wall -g -fsanitize=undefined -fopenmp -fPIC `python3 -m pybind11 --includes` src/*.cpp -I include -std=c++2a;;

	Darwin*)
		gcc -Wall -g -fsanitize=undefined -Xpreprocessor -fopenmp -fPIC `python3 -m pybind11 --includes` src/*.cpp -I include -L/usr/local/lib -lomp -lstdc++ -std=c++2a;;

esac

mv a.out bin/debug/
