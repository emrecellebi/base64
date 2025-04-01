@echo off

gcc -c ./src/test/main.c -I ./include
gcc -o ./bin/b64.exe main.o -L ./lib -lbase64
gcc -o ./bin/_b64.exe main.o -L ./lib -lbase64lib

del *.o