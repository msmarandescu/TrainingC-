
cmake_minimum_required(VERSION 3.0)

#create client build:
add_executable(client.exe client.cpp SocketCommunication.cpp)

#create server build:
add_executable(server.exe server.cpp SocketCommunication.cpp)

