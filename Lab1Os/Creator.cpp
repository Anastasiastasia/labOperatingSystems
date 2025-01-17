#include"Header.h"
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uncorrect!" << std::endl;
        return 1;
    }
    const char* filename = argv[1];
    int recordCount = std::stoi(argv[2]);
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open file for writing!" << std::endl;
        return 1;
    }
    for (int i = 0; i < recordCount; ++i) {
        employee emp;
        std::cout << "Enter employee number: ";
        std::cin >> emp.num;
        std::cout << "Enter the employee's name: ";
        std::cin >> emp.name;
        std::cout << "Enter the number of hours worked: ";
        std::cin >> emp.hours;
        outFile.write(reinterpret_cast<const char*>(&emp), sizeof(employee));
    }

    outFile.close();
    return 0;
}
