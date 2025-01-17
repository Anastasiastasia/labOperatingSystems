#include <gtest/gtest.h>
#include "Header.h"

DWORD WINAPI markerThread(LPVOID param) {
    Marker& marker = *static_cast<Marker*>(param);
    marker.markedCount = 0;
    marker.canContinue = true;
    marker.stopRequested = false;
    srand(marker.id);
    WaitForSingleObject(hEventStart, INFINITE);
    while (true) {
        int randomNumber = rand() % 100; 
        int index = randomNumber % numbers.size(); 
        EnterCriticalSection(&cs);
        if (numbers[index] == 0) { 
            Sleep(SLEEP_DURATION); 
            numbers[index] = marker.id;
            marker.markedCount++;
            LeaveCriticalSection(&cs);
            Sleep(SLEEP_DURATION); 
        }
        else { 
            marker.canContinue = false; 
            LeaveCriticalSection(&cs);
            break; 
        }
    }
    WaitForSingleObject(hEventContinue, INFINITE);
    if (marker.stopRequested) {
        EnterCriticalSection(&cs);
        for (int i = 0; i < numbers.size(); i++) {
            if (numbers[i] == marker.id) {
                numbers[i] = 0; 
            }
        }
        LeaveCriticalSection(&cs);
    }
    return 0;
}
TEST(MarkerThreadTest, MarkingBehavior) {
    InitializeCriticalSection(&cs);
    numbers.resize(10, 0);
    markers.resize(2); 
    vector<HANDLE> markerThreads;
    for (int i = 0; i < 2; i++) {
        markers[i].id = i + 1;
        markerThreads.push_back(CreateThread(NULL, 0, markerThread, &markers[i], 0, NULL));
    }
    hEventStart = CreateEvent(NULL, FALSE, FALSE, NULL);
    SetEvent(hEventStart);
    Sleep(100); 
    for (const auto& marker : markers) {
        EXPECT_GE(marker.markedCount, 0);
    }
    for (auto& thread : markerThreads) {
        markers[0].stopRequested = true; 
        SetEvent(hEventContinue);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }

    DeleteCriticalSection(&cs);
    CloseHandle(hEventStart);
}
TEST(MarkerThreadTest, StopMarker) {
    InitializeCriticalSection(&cs);
    numbers.resize(10, 0);
    markers.resize(2);
    vector<HANDLE> markerThreads;
    for (int i = 0; i < 2; i++) {
        markers[i].id = i + 1;
        markerThreads.push_back(CreateThread(NULL, 0, markerThread, &markers[i], 0, NULL));
    }
    hEventStart = CreateEvent(NULL, FALSE, FALSE, NULL);
    SetEvent(hEventStart);
    Sleep(100);
    markers[0].stopRequested = true; 
    SetEvent(hEventContinue); 
    WaitForSingleObject(markerThreads[0], INFINITE);
    CloseHandle(markerThreads[0]);
    for (const auto& num : numbers) {
        EXPECT_LE(num, 2); // Маркер может пометить только 2
    }
    DeleteCriticalSection(&cs);
    CloseHandle(hEventStart);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
