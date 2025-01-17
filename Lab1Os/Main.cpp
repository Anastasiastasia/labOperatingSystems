#include <iostream>
#include <windows.h>
#include <string>
void runProcess(const std::wstring& command) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    if (!CreateProcess(NULL, const_cast<LPWSTR>(command.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::wcerr << L"Failed to start process: " << command << std::endl;
        return;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main() {
    std::wstring binaryFilename;
    int recordCount;
    std::wcout << L"Enter the name of the binary file: ";
    std::wcin >> binaryFilename;
    std::wcout << L"Enter the number of records: ";
    std::wcin >> recordCount;
    std::wstring creatorCommand = L"Creator.exe \"" + binaryFilename + L"\" " + std::to_wstring(recordCount);
    runProcess(creatorCommand);
    std::wstring reportFilename;
    double hourlyRate;

    std::wcout << L"Report filename: ";
    std::wcin >> reportFilename;

    std::wcout << L"Hourly rate: ";
    std::wcin >> hourlyRate;
    std::wstring reporterCommand = L"Reporter.exe \"" + binaryFilename + L"\" \"" + reportFilename + L"\" " + std::to_wstring(hourlyRate);
    runProcess(reporterCommand);
    std::wcout << L"The process is complete." << std::endl;
    return 0;
}
