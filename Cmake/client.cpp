#include "SocketCommunication.h"

#define socketInstance SocketCommunication::getInstance()

int main() {
    std::string ipAddress = "127.0.0.1";
    int port = 9080;
    std::string logFileName = "clientLogFile.log";

    std::this_thread::sleep_for(std::chrono::seconds(3)); // Delay for synchronization

    int clientSocket = socketInstance.createSocket();

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ipAddress.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address/ Address not supported" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (connect(clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string message = "what's the date?";
    socketInstance.sendData(clientSocket, message);
    socketInstance.saveToLog(logFileName, "Client writes to server: " + message + "\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::string serverResponse = socketInstance.receiveData(clientSocket);
    socketInstance.saveToLog(logFileName, "Client received response from server: " + serverResponse + "\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    close(clientSocket);
    socketInstance.saveToLog(logFileName, "Client closing!\n");

    return 0;
}