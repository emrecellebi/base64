@echo off

g++ -c ./src/test/main.cpp -I ./include
g++ -o ./bin/b64.exe main.o -L ./lib -lbase64
g++ -o ./bin/_b64.exe main.o -L ./lib -lbase64lib

del *.o