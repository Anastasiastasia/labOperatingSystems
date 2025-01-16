#include <gtest/gtest.h>
#include <windows.h>
#include <thread>
#include <mutex>
#include <iostream>
using namespace std;
const wchar_t* REQUEST_PIPE = L"\\\\.\\pipe\\request_pipe";
const wchar_t* RESPONSE_PIPE = L"\\\\.\\pipe\\response_pipe";
const wchar_t* FILENAME = L"employees.bin";

struct employee {
    int num;
    char name[10];
    double hours;
};
mutex fileMutex;
void createEmployeeFile();
void handleClientRequests();
void requestEmployeeRecord();
TEST(EmployeeTest, CreateEmployeeFile) {
    DeleteFile(FILENAME);
    thread t(createEmployeeFile);
    t.join(); 
    HANDLE hFile = CreateFile(
        FILENAME,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    ASSERT_NE(hFile, INVALID_HANDLE_VALUE);
    CloseHandle(hFile);
}
TEST(EmployeeTest, RequestEmployeeRecord) {
    createEmployeeFile();
    thread serverThread(handleClientRequests);
    serverThread.detach();
    employee empTest = { 1, "John", 40.0 };
    HANDLE requestPipe = CreateFile(REQUEST_PIPE, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (requestPipe != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        WriteFile(requestPipe, &empTest, sizeof(employee), &bytesWritten, NULL);
        CloseHandle(requestPipe);
    }
    employee responseEmp;
    HANDLE responsePipe = CreateFile(RESPONSE_PIPE, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (responsePipe != INVALID_HANDLE_VALUE) {
        DWORD bytesRead;
        ReadFile(responsePipe, &responseEmp, sizeof(employee), &bytesRead, NULL);
        CloseHandle(responsePipe);
        EXPECT_EQ(responseEmp.num, empTest.num);
        EXPECT_STREQ(responseEmp.name, empTest.name);
        EXPECT_EQ(responseEmp.hours, empTest.hours);
    }
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}