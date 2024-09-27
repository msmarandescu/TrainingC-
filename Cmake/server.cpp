#include "SocketCommunication.h"

#define socketInstance SocketCommunication::getInstance()

int main() {
    std::string ipAddress = "127.0.0.1";
    int port = 9080;
    std::string logFileName = "serverLogFile.log";

    int serverSocket = socketInstance.createSocket();
    socketInstance.bindSocket(serverSocket, port);
    socketInstance.listenSocket(serverSocket, 3);

    socketInstance.saveToLog(logFileName, "Server started! Waiting for client request...\n");

    int clientSocket = socketInstance.acceptConnection(serverSocket);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::string clientMessage = socketInstance.receiveData(clientSocket);
    socketInstance.saveToLog(logFileName, "Server received message from client: " + clientMessage + "\n");

    if (clientMessage == "what's the date?") {
        std::string dateResponse = socketInstance.getCurrentDateTime();
        socketInstance.sendData(clientSocket, dateResponse);
        socketInstance.saveToLog(logFileName, "Server sent response to client: " + dateResponse + "\n");
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));

    close(clientSocket);
    close(serverSocket);
    socketInstance.saveToLog(logFileName, "Server closing!\n");

    return 0;
}