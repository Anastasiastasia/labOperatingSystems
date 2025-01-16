#include <iostream>
#include <windows.h>
#include <vector>
#include <limits>
struct ThreadData {
    const std::vector<int>* arr;
    double average;
};
DWORD WINAPI findMinMax(LPVOID param) {
    ThreadData* data = static_cast<ThreadData*>(param);
    const std::vector<int>& arr = *data->arr;
    if (arr.empty()) {
        std::cerr << "Array is empty" << std::endl;
        return 1;
    }
    int minVal = arr[0];
    int maxVal = arr[0];
    for (const auto& num : arr) {
        if (num < minVal) {
            minVal = num;
        }
        Sleep(7);
        if (num > maxVal) {
            maxVal = num;
        }
        Sleep(7);
    }
    std::cout << "Min: " << minVal << ", Max: " << maxVal << std::endl;
    return 0;
}
DWORD WINAPI calculateAverage(LPVOID param) {
    ThreadData* data = static_cast<ThreadData*>(param);
    const std::vector<int>& arr = *data->arr;
    if (arr.empty()) {
        std::cerr << "Array is empty" << std::endl;
        return 1;
    }
    double sum = 0;
    for (const auto& num : arr) {
        sum += num;
        Sleep(12);
    }
    data->average = sum / arr.size();
    std::cout << "Average: " << data->average << std::endl;
    return 0;
}
