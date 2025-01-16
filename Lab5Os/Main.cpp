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
void createEmployeeFile() {
    HANDLE hFile = CreateFile(
        FILENAME,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "Error creating file." << endl;
        return;
    }
    int n;
    cout << "Enter number of employees: ";
    cin >> n;
    for (int i = 0; i < n; ++i) {
        employee emp;
        cout << "Enter ID, Name and Hours for Employee " << (i + 1) << ": ";
        cin >> emp.num >> emp.name >> emp.hours;

        DWORD bytesWritten;
        WriteFile(hFile, &emp, sizeof(employee), &bytesWritten, NULL);
    }

    CloseHandle(hFile);
    cout << "Employee file created." << endl;
}
void handleClientRequests() {
    HANDLE requestPipe = CreateNamedPipe(
        REQUEST_PIPE,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        1,
        sizeof(employee),
        sizeof(employee),
        0,
        NULL
    );

    if (requestPipe == INVALID_HANDLE_VALUE) {
        cerr << "Error creating request named pipe." << endl;
        return;
    }
    cout << "Server is waiting for client connection..." << endl;
    while (true) {
        if (ConnectNamedPipe(requestPipe, NULL) != FALSE) {
            employee emp;
            DWORD bytesRead;
            ReadFile(requestPipe, &emp, sizeof(employee), &bytesRead, NULL);
            cout << "Received request for ID: " << emp.num << endl;
            fileMutex.lock();
            HANDLE hFile = CreateFile(
                FILENAME,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );

            bool found = false;
            employee foundEmp;
            DWORD bytesWritten;
            DWORD totalBytesRead = 0;

            while (ReadFile(hFile, &foundEmp, sizeof(employee), &bytesRead, NULL) && bytesRead > 0) {
                if (foundEmp.num == emp.num) {
                    found = true;
                    emp = foundEmp; 
                    break;
                }
                totalBytesRead += bytesRead;
            }
            if (found) {
                cout << "Employee found. Please provide new data." << endl;
                cout << "Enter new Name and Hours: ";
                cin >> emp.name >> emp.hours;
                SetFilePointer(hFile, totalBytesRead - sizeof(employee), NULL, FILE_BEGIN);
                WriteFile(hFile, &emp, sizeof(employee), &bytesWritten, NULL);
            }
            CloseHandle(hFile);
            fileMutex.unlock();
            HANDLE responsePipe = CreateFile(
                RESPONSE_PIPE,
                GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
            );
            if (responsePipe != INVALID_HANDLE_VALUE) {
                DWORD bytesWritten;
                WriteFile(responsePipe, &emp, sizeof(employee), &bytesWritten, NULL);
                CloseHandle(responsePipe);
            }
            else {
                cerr << "Error opening response named pipe." << endl;
            }
        }
        DisconnectNamedPipe(requestPipe); 
    }
    CloseHandle(requestPipe);
}
void requestEmployeeRecord() {
    int id;
    cout << "Enter employee ID to request: ";
    cin >> id;

    employee emp;
    emp.num = id;
    HANDLE requestPipe = CreateFile(
        REQUEST_PIPE,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (requestPipe != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        WriteFile(requestPipe, &emp, sizeof(employee), &bytesWritten, NULL);
        CloseHandle(requestPipe);
    }
    else {
        cerr << "Error opening request named pipe." << endl;
        return;
    }
    employee responseEmp;
    HANDLE responsePipe = CreateNamedPipe(
        RESPONSE_PIPE,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        1,
        sizeof(employee),
        sizeof(employee),
        0,
        NULL
    );

    if (responsePipe == INVALID_HANDLE_VALUE) {
        cerr << "Error creating response named pipe." << endl;
        return;
    }
    if (ConnectNamedPipe(responsePipe, NULL) != FALSE) {
        DWORD bytesRead;
        ReadFile(responsePipe, &responseEmp, sizeof(employee), &bytesRead, NULL);
        CloseHandle(responsePipe);

        cout << "Received Employee Record: ID: " << responseEmp.num << ", Name: " << responseEmp.name << ", Hours: " << responseEmp.hours << endl;
    }
    else {
        cerr << "Error connecting to response named pipe." << endl;
        CloseHandle(responsePipe);
    }
}
int main() {
    createEmployeeFile();
    thread serverThread(handleClientRequests);
    serverThread.detach();
    while (true) {
        int choice;
        cout << "1. Request Employee Record\n";
        cout << "2. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            requestEmployeeRecord();
            break;
        case 2:
            return 0;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }
    return 0;
}
