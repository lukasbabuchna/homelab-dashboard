#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <tuple>
#include <regex>

char *memoryFilePath = "/proc/meminfo";
char *temperatureMemoryPath = "/sys/class/thermal/thermal_zone0/temp";

int getTemperature()
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

std::tuple<int, int> getMemory()
{
    std::ifstream memoryFile(memoryFilePath);

    if (!memoryFile.is_open())
    {
        std::cout << "Couldn't open the memory file \n";
        return std::make_tuple(0, 0);
    }

    std::string rawTotal;
    std::string rawFree;

    getline(memoryFile, rawTotal); // MemTotal:        8255888 kB
    getline(memoryFile, rawFree);  // MemFree:         6814080 kB

    // TODO: use regex to extract ints or pass the whole string to Python to parse it
}

int main()
{

    std::cout << getTemperature() << "\n";
    return 0;
}