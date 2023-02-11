all:
	g++ ./src/*.cpp -o ./bin/raycasting.exe -O2 -fopenmp -Wall -I include/SDL2 -I include/Raycasting -L lib -lmingw32 -lSDL2main -lSDL2
