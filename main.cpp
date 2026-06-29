#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <tuple>
#include <regex>

int getTemperature(std::string temperatureMemoryPath)
{
    std::ifstream temperatureFile(temperatureMemoryPath);

    if (!temperatureFile.is_open())
    {
        std::cout << "Couldn't open the temperature file \n";
        return 0;
    }

    std::string temperatureStr;

    temperatureFile >> temperatureStr;

    int temperature = stoi(temperatureStr);

    // 54°C outputed as 54000
    return temperature;
}

std::tuple<int, int> getMemory(std::string memoryFilePath)
{
    std::ifstream memoryFile(memoryFilePath);

    if (!memoryFile.is_open())
    {
        std::cout << "Couldn't open the memory file" << "\n";
        return std::make_tuple(0, 0);
    }

    std::string rawTotal;
    std::string rawFree;

    getline(memoryFile, rawTotal); // MemTotal:        8255888 kB
    getline(memoryFile, rawFree);  // MemFree:         6814080 kB

    std::smatch match;
    std::regex regex(R"(\d+)");

    int totalMem;
    int freeMem;

    // Regex on total

    if (std::regex_search(rawTotal, match, regex))
    {
        totalMem = std::stoi(match.str());
    }
    else
    {
        std::cout << "Could find number in total memory string" << "\n";
        totalMem = 0;
    }

    // Regex on free

    if (std::regex_search(rawFree, match, regex))
    {
        freeMem = std::stoi(match.str());
    }
    else
    {
        std::cout << "Could find number in total memory string" << "\n";
        freeMem = 0;
    }

    // total memory , free memory

    return std::tuple(totalMem, freeMem); // values in kB
}

std::string getHostname()
{
    std::ifstream hostnameFile("/etc/hostname");

    std::string hostname;

    if (!hostnameFile.is_open())
    {
        std::cout << "Couldn't open the memory file" << "\n";
        return "Unknown";
    }

    getline(hostnameFile, hostname);

    return hostname;
}

int main()
{

    std::string memoryFilePath = "/proc/meminfo";
    std::string temperatureMemoryPath = "/sys/class/thermal/thermal_zone0/temp";

    std::cout << getTemperature(temperatureMemoryPath) << "\n";

    std::tuple<int, int> memoryData = getMemory(memoryFilePath);

    int totalMemory = std::get<0>(memoryData);
    int freeMemory = std::get<1>(memoryData);

    std::cout << totalMemory << "\n"
              << freeMemory << "\n";

    std::cout << getHostname() << "\n";
    return 0;
}