#include <iostream>
#include <fstream>
#include <future>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <string>
#include <boost/signals2.hpp>

using namespace boost;
using namespace boost::signals2;


template <typename T> class Observable {
public:
    boost::signals2::signal<void(T&, const std::string&, const int& taskId)> field_changed;
};

class fileReader : public Observable<fileReader>
{
private:
    fileReader(){}
    std::mutex mtx;
public:
    fileReader(fileReader const&) = delete;
    void operator=(fileReader const&) = delete;

    static fileReader& getInstance()
    {
        static fileReader fr;
        return fr;
    }

    std::string readCommand(const std::string& fileName)
    {
        std::string message = "Eroare la deschiderea fișierului";
        std::lock_guard<std::mutex> lock(mtx);
        std::ifstream readStream(fileName);
        if(readStream.is_open())
        {
            getline(readStream, message);
            readStream.close();
        }
        else
        {
            std::cout << "Eroare la deschiderea fișierului \n";
        }
        field_changed(*this, "readCommand()", getThreadId());
        return message;
    }

    void writeFile(const std::string& fileName, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::ofstream writeStream(fileName,std::ofstream::trunc); //open in overwrite mode
        if(!writeStream.is_open())
        {
            std::cout << "Eroare la deschiderea fișierului: " << fileName << std::endl;
            return;
        }

        writeStream << message;
        writeStream.close();
        field_changed(*this, "writeFile()", getThreadId());
    }

    void addToLog(std::string fileName, std::string mes)
    {
        std::ofstream os(fileName, std::ios::app); //open in append mode
        if(!os.is_open())
        {
            std::cout << "Eroare la deschiderea fisierului: " << fileName << std::endl;
            return;
        }
        os << mes;
        field_changed(*this, "addToLog()", getThreadId());
        os.close();
    }

    int getThreadId()
    {
        std::thread::id threadId = std::this_thread::get_id();
        size_t hashValue = std::hash<std::thread::id>{}(threadId);
        int intThreadId = std::abs(static_cast<int>(hashValue));

        return intThreadId;
    }

};

#define fileInstance fileReader::getInstance()
#define logServer(message) fileInstance.addToLog("server.log", message)
#define logClient(message) fileInstance.addToLog("client.log", message)

bool clientReady = false;
bool serverReady = false;
std::mutex mtx;
std::condition_variable cv;



void server(const std::string sharedFileName)
{
    logServer("Server started!\nWaiting for client request...\n");
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&](){ return clientReady; });

    if(fileInstance.readCommand(sharedFileName) == "what's the date?")
    {
        logServer("Server reacived message from client: " + fileInstance.readCommand(sharedFileName) + "\n");
        serverReady = false;
        system(("date | head --bytes=38  > " + sharedFileName).c_str());
        logServer("Server sent message to client: " + fileInstance.readCommand(sharedFileName) + "\n");
        serverReady = true;
        cv.notify_one();
    }
    logServer("Server cloaseing!\n");
}

void client(const std::string& sharedFileName)
{

    logClient("Client started!\n");
    std::unique_lock<std::mutex> lock(mtx);
    clientReady = false;
    fileInstance.writeFile(sharedFileName, "what's the date?");
    logClient("Client writes to server: " + fileInstance.readCommand(sharedFileName) + "\n");
    clientReady = true;
    cv.notify_one();

    logClient("Client waiting response from server...\n");
    cv.wait(lock, [](){ return serverReady; });
    logClient("Client reacived response from server: " + fileInstance.readCommand(sharedFileName) + "\n");
    logClient("Client closing!\n");
    
}

int main() {

    std::string sharedFileName = "sharedFile.txt";
    fileInstance.writeFile("sharedFile.txt", "");
    fileInstance.writeFile("client.log", "");
    fileInstance.writeFile("server.log", "");

    auto conn = fileInstance.field_changed.connect(
        [](fileReader& obj, const std::string& field_name, const int& taskId)
        {
            std::cout << field_name << " method was invoked by Task Id: " << taskId << std::endl;
        }
    );

    std::future<void> task1 = std::async(std::launch::async, server, sharedFileName);
    std::future<void> task2 = std::async(std::launch::async, client, sharedFileName);
    

    task1.get();
    task2.get();

    return 0;
}
