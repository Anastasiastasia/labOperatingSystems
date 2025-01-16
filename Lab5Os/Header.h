#include <iostream>
#include <thread>
#include <mutex>
#include <windows.h>
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
