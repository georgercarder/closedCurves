CC=g++
STD=c++17

closed_curves: closed_curves.cpp
	$(CC) -std=$(STD) -O3 -o closed_curves closed_curves.cpp
