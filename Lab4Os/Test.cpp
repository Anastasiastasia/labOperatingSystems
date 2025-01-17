#include <gtest/gtest.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <sstream>
extern const int MAX_MESSAGE_LENGTH;
extern const int QUEUE_SIZE;
extern std::mutex mtx;
extern std::condition_variable cv;
extern std::vector<std::string> messageQueue;
extern int writeIndex;
extern int readIndex;
extern bool ready;
void test_receiver() {
    std::string testMessage = "Hello";
    {
        std::lock_guard<std::mutex> lock(mtx);
        messageQueue[writeIndex] = testMessage;
        writeIndex = (writeIndex + 1) % QUEUE_SIZE;
        ready = true;
    }
    cv.notify_one();
    std::string receivedMessage;
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return writeIndex != readIndex; });
        receivedMessage = messageQueue[readIndex];
        readIndex = (readIndex + 1) % QUEUE_SIZE;
        ready = (writeIndex != readIndex);
    }
    EXPECT_EQ(receivedMessage, testMessage);
}

void test_sender() {
    std::string testMessage = "Test message";
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return (writeIndex + 1) % QUEUE_SIZE != readIndex; });

        messageQueue[writeIndex] = testMessage;
        writeIndex = (writeIndex + 1) % QUEUE_SIZE;
        ready = true;
    }
    cv.notify_one();
    EXPECT_EQ(messageQueue[(writeIndex - 1 + QUEUE_SIZE) % QUEUE_SIZE], testMessage);
}

TEST(MessagePassingTest, ReceiverTest) {
    test_receiver();
}
TEST(MessagePassingTest, SenderTest) {
    test_sender();
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
