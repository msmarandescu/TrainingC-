#include <iostream>
#include <cstring>
#include <future>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>

class SocketCommunication
{
    SocketCommunication(){}
public:
    SocketCommunication(SocketCommunication const&) = delete;
    void operator=(SocketCommunication const&) = delete;

    static SocketCommunication& getInstance();
    int createSocket();  

    void bindSocket(int sockfd, int port);

    void listenSocket(int sockfd, int backlog);

    int acceptConnection(int sockfd);

    void sendData(int sockfd, const std::string& data);

    std::string receiveData(int sockfd);

    std::string getCurrentDateTime();

    void saveToLog(std::string fileName, std::string mes);
};
