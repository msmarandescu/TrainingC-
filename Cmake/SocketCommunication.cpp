#include "SocketCommunication.h"


    SocketCommunication& SocketCommunication::getInstance()
    {
        static SocketCommunication instance;
        return instance;
    }

    int SocketCommunication::createSocket() {
        int sockfd;
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            std::cout << "Socket creation error" << std::endl;
            exit(EXIT_FAILURE);
        }
        return sockfd;
    }   

    void SocketCommunication::bindSocket(int sockfd, int port) {
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        // Setarea opțiunii SO_REUSEADDR (cad rulez aplicatia imediat dupa ce a fost inchisa, portul este încă rezervat de procesul anterior)
        int opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            std::cout << "Setsockopt failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            std::cout << "Bind failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void SocketCommunication::listenSocket(int sockfd, int backlog) {
        if (listen(sockfd, backlog) < 0) {
            std::cout << "Listen failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    int SocketCommunication::acceptConnection(int sockfd) {
        int new_socket;
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        if ((new_socket = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cout << "Acceptance failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        return new_socket;
    }

    void SocketCommunication::sendData(int sockfd, const std::string& data) {
        send(sockfd, data.c_str(), data.length(), 0);
    }

    std::string SocketCommunication::receiveData(int sockfd) {
        char buffer[1024] = {0};
        read(sockfd, buffer, 1024);
        return std::string(buffer);
    }

    std::string SocketCommunication::getCurrentDateTime() {
        time_t now = time(0);
        struct tm* localTime = localtime(&now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
        return std::string(buffer);
    }

    void SocketCommunication::saveToLog(std::string fileName, std::string mes)
    {
        std::ofstream os(fileName, std::ios::app); //open in append mode
        if(!os.is_open())
        {
            std::cout << "Eroare la deschiderea fisierului: " << fileName << std::endl;
            return;
        }
        os << getCurrentDateTime() + ":" + mes;
        os.close();
    }

