#include "Header.h"
int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Uncorrect!" << std::endl;
        return 1;
    }
    const char* inputFilename = argv[1];
    const char* reportFilename = argv[2];
    double hourlyRate = std::stod(argv[3]);
    std::ifstream inFile(inputFilename, std::ios::binary);
    std::ofstream reportFile(reportFilename);
    if (!inFile || !reportFile) {
        std::cerr << "Failed to open file!" << std::endl;
        return 1;
    }
    reportFile << "File report \"" << inputFilename << "\"\n";
    reportFile << "Employee Number, Employee Name, Hours, Salary\n";
    employee emp;
    while (inFile.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        double salary = emp.hours * hourlyRate;
        reportFile << emp.num << ", " << emp.name << ", " << emp.hours << ", " 
                   << salary << "\n";
    }
    inFile.close();
    reportFile.close();
    return 0;
}
