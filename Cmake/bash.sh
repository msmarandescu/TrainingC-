#!/bin/bash

rm -fr out/

cmake -S . -B out/build/
cd out/build/
make

#run server and client:
echo "\nExecuting files: server.exe and client.exe\n"
./server.exe & ./client.exe &
