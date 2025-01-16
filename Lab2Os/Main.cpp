#include"Header.h"
int main() {
    try {
        int size;
        std::cout << "Enter the size of the array: ";
        std::cin >> size;
        if (size <= 0) {
            throw std::invalid_argument("Size must be greater than 0");
        }
        std::vector<int> arr(size);
        std::cout << "Enter " << size << " elements:\n";
        for (int i = 0; i < size; ++i) {
            std::cin >> arr[i];
        }
        ThreadData data;
        data.arr = &arr;
        HANDLE hMinMaxThread = CreateThread(NULL, 0, findMinMax, &data, 0, NULL);
        HANDLE hAverageThread = CreateThread(NULL, 0, calculateAverage, &data, 0, NULL);
        if (hMinMaxThread == NULL || hAverageThread == NULL) {
            std::cerr << "Error creating threads." << std::endl;
            return 1;
        }
        WaitForSingleObject(hMinMaxThread, INFINITE);
        WaitForSingleObject(hAverageThread, INFINITE);
        CloseHandle(hMinMaxThread);
        CloseHandle(hAverageThread);
        int minVal = arr[0];
        int maxVal = arr[0];
        for (const auto& num : arr) {
            if (num < minVal) {
                minVal = num;
            }
            if (num > maxVal) {
                maxVal = num;
            }
        }
        for (auto& num : arr) {
            if (num == minVal || num == maxVal) {
                num = static_cast<int>(data.average);
            }
        }
        std::cout << "Modified array: ";
        for (const auto& num : arr) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
