#include <iostream>
#include <fstream>
#include <string>
#include <set>

// Helper function to read the first line of a file
std::string readFirstLine(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    if (file.is_open() && std::getline(file, line)) {
        file.close();
        return line;
    }
    return "Unknown";
}

// Get OS version from /etc/os-release
void getOSVersion() {
    std::ifstream file("/etc/os-release");
    std::string line;
    std::cout << "OS Version: ";
    while (std::getline(file, line)) {
        if (line.find("PRETTY_NAME=") != std::string::npos) {
            std::cout << line.substr(line.find("=") + 1) << std::endl;
            return;
        }
    }
    std::cout << "Unknown" << std::endl;
}

// Get CPU information from /proc/cpuinfo
void getCPUInfo() {
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    std::string modelName = "Unknown";
    int totalThreads = 0;
    std::set<int> physicalCores;

    while (std::getline(file, line)) {
        if (line.find("model name") != std::string::npos && modelName == "Unknown") {
            modelName = line.substr(line.find(":") + 2);
        } else if (line.find("processor") != std::string::npos) {
            totalThreads++;
        } else if (line.find("core id") != std::string::npos) {
            int coreId = std::stoi(line.substr(line.find(":") + 2));
            physicalCores.insert(coreId);
        }
    }

    std::cout << "CPU Model: " << modelName << std::endl;
    std::cout << "Total Threads: " << totalThreads << std::endl;
    std::cout << "Total Cores: " << physicalCores.size() << std::endl;
}

// Get total RAM from /proc/meminfo
void getRAMInfo() {
    std::ifstream file("/proc/meminfo");
    std::string line;
    long totalRAM = 0;

    while (std::getline(file, line)) {
        if (line.find("MemTotal") != std::string::npos) {
            totalRAM = std::stol(line.substr(line.find(":") + 1));
            break;
        }
    }

    std::cout << "Main RAM: " << totalRAM / 1024 << " MB" << std::endl;
}

int main() {
    std::cout << "System Configuration:\n";
    getOSVersion();
    getCPUInfo();
    getRAMInfo();
    return 0;
}
