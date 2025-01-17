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
            cout << "Marker " << marker.id << " cannot mark index " << index << endl;
            cout << "Marked count: " << marker.markedCount << endl;
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

int main() {
    int size, numMarkers;
    cout << "Enter size of the array: ";
    cin >> size;
    cout << "Enter number of marker threads (max " << MAX_MARKERS << "): ";
    cin >> numMarkers;
    if (numMarkers > MAX_MARKERS) numMarkers = MAX_MARKERS;
    numbers.resize(size, 0);
    markers.resize(numMarkers);
    InitializeCriticalSection(&cs);
    hEventStart = CreateEvent(NULL, FALSE, FALSE, NULL);
    hEventContinue = CreateEvent(NULL, FALSE, FALSE, NULL);
    hEventStop = CreateEvent(NULL, FALSE, FALSE, NULL);
    vector<HANDLE> markerThreads;
    for (int i = 0; i < numMarkers; i++) {
        markers[i].id = i + 1;
        markerThreads.push_back(CreateThread(NULL, 0, markerThread, &markers[i], 0, NULL));
    }
    SetEvent(hEventStart);
    while (true) {
        bool allStopped = true;
        for (const auto& marker : markers) {
            if (marker.canContinue) {
                allStopped = false;
                break;
            }
        }
        if (allStopped) {
            cout << "Array contents: ";
            for (const auto& num : numbers) {
                cout << num << " ";
            }
            cout << endl;
            int stopIndex;
            cout << "Enter the marker number to stop: ";
            cin >> stopIndex;
            if (stopIndex > 0 && stopIndex <= numMarkers) {
            
                markers[stopIndex - 1].stopRequested = true;
                SetEvent(hEventContinue); 
                WaitForSingleObject(markerThreads[stopIndex - 1], INFINITE);
                cout << "Array contents after stopping marker " << stopIndex << ": ";
                for (const auto& num : numbers) {
                    cout << num << " ";
                }
                cout << endl;
                SetEvent(hEventContinue);
            }
            else {
                cout << "Invalid marker number!" << endl;
            }
        }
    }
    for (auto& thread : markerThreads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
    DeleteCriticalSection(&cs);
    CloseHandle(hEventStart);
    CloseHandle(hEventContinue);
    CloseHandle(hEventStop);
    return 0;
}
