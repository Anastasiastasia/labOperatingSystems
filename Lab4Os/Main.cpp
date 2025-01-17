#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
const int MAX_MESSAGE_LENGTH = 20;
const int QUEUE_SIZE = 5; 
std::mutex mtx;
std::condition_variable cv;
std::vector<std::string> messageQueue(QUEUE_SIZE);
int writeIndex = 0;
int readIndex = 0;
bool ready = false;
void receiver(const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Unable to create file: " << filename << std::endl;
        return;
    }
    std::cout << "Receiver is ready and waiting for messages " << std::endl;
    std::string command;
    while (true) {
        std::cout << "Enter 'read' to read a message or 'exit' to quit: ";
        std::cin >> command;
        if (command == "read") {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] { return writeIndex != readIndex; });
            std::string message = messageQueue[readIndex];
            readIndex = (readIndex + 1) % QUEUE_SIZE;

            std::cout << "Received message: " << message << std::endl;
            if (writeIndex == readIndex) {
                ready = false;
            }
            cv.notify_one(); 
        }
        else if (command == "exit") {
            break;
        }
    }

    outFile.close();
}
void sender() {
    std::string message;
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
        cv.notify_all();
    }
    while (true) {
        std::cout << "Enter message (max " << MAX_MESSAGE_LENGTH << " characters) or 'exit' to quit: ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        if (message.length() > MAX_MESSAGE_LENGTH) {
            std::cout << "Message too long! Please limit to " << MAX_MESSAGE_LENGTH << " characters." << std::endl;
            continue;
        }
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return (writeIndex + 1) % QUEUE_SIZE != readIndex; });
        messageQueue[writeIndex] = message;
        writeIndex = (writeIndex + 1) % QUEUE_SIZE;
        ready = true;
        cv.notify_one(); 
    }
}
int main() {
    std::string filename;

    std::cout << "Enter name of the binary file: ";
    std::cin >> filename;
    std::cin.ignore(); 
    std::thread receiverThread(receiver, filename);
    std::thread senderThread(sender);

    receiverThread.join();
    senderThread.join();

    return 0;
}
