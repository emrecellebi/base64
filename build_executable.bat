@echo off

g++ -c ./src/test/main.cpp -I ./include
g++ -o ./bin/b64.exe *.o -L ./lib -lbase64