case "$(uname -sr)" in

	Linux*)
		g++ -Wall -g -O3 -march=native -ffast-math -fopenmp -fPIC `python3 -m pybind11 --includes` src/*.cpp -I include -lstdc++ -std=c++2a;;

	Darwin*)
		g++ -Wall -g -Xpreprocessor -fopenmp -fPIC `python3 -m pybind11 --includes` src/*.cpp -I include -L/usr/local/lib -lomp -lstdc++ -std=c++2a;;

esac

mv a.out bin/debug/
./bin/debug/a.out
