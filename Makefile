CC=g++
STD=c++17

all: closed_curves closed_curves_parallel

closed_curves: closed_curves.cpp
	$(CC) -std=$(STD) -O3 -pthread -o closed_curves closed_curves.cpp

closed_curves_parallel: closed_curves_parallel.cpp
	$(CC) -std=$(STD) -O3 -pthread -o closed_curves_parallel closed_curves_parallel.cpp



#$(CC) -std=$(STD) -pthread -O3 -o closed_curves closed_curves.cpp
