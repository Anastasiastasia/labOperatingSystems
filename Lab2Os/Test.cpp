#include <gtest/gtest.h>
#include"Header.h"
TEST(MinMaxTest, HandlesPositiveNumbers) {
    std::vector<int> arr = { 1, 2, 3, 4, 5 };
    ThreadData data;
    data.arr = &arr;

    HANDLE hThread = CreateThread(NULL, 0, findMinMax, &data, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    ASSERT_EQ(data.minVal, 1);
    ASSERT_EQ(data.maxVal, 5);
}
TEST(MinMaxTest, HandlesNegativeNumbers) {
    std::vector<int> arr = { -5, -3, -4, -1, -2 };
    ThreadData data;
    data.arr = &arr;

    HANDLE hThread = CreateThread(NULL, 0, findMinMax, &data, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    ASSERT_EQ(data.minVal, -5);
    ASSERT_EQ(data.maxVal, -1);
}
TEST(AverageTest, HandlesPositiveNumbers) {
    std::vector<int> arr = { 1, 2, 3, 4, 5 };
    ThreadData data;
    data.arr = &arr;

    HANDLE hThread = CreateThread(NULL, 0, calculateAverage, &data, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    ASSERT_DOUBLE_EQ(data.average, 3.0);
}
TEST(AverageTest, HandlesMixedNumbers) {
    std::vector<int> arr = { -1, 0, 1 };
    ThreadData data;
    data.arr = &arr;

    HANDLE hThread = CreateThread(NULL, 0, calculateAverage, &data, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    ASSERT_DOUBLE_EQ(data.average, 0.0);
}
TEST(AverageTest, HandlesEmptyArray) {
    std::vector<int> arr = {};
    ThreadData data;
    data.arr = &arr;

    ASSERT_THROW({ calculateAverage(&data); }, std::runtime_error);
}
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
