#include <windows.h>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

using namespace std;

const int MAX_MARKERS = 10;
const int SLEEP_DURATION = 5; 
CRITICAL_SECTION cs;
HANDLE hEventStart; 
HANDLE hEventContinue; 
HANDLE hEventStop; 

struct Marker {
    int id;
    int markedCount;
    bool canContinue;
    bool stopRequested;
};

vector<int> numbers;
vector<Marker> markers;
