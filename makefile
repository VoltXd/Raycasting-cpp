all:
	g++ ./src/*.cpp -o ./bin/raycasting.exe -Wall -I include/SDL2 -I include/Raycasting -L lib -lmingw32 -lSDL2main -lSDL2
