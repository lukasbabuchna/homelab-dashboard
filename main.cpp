#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <tuple>
#include <regex>
#include <netinet/in.h>
#include <sys/socket.h>

int getTemperature(std::string temperaturePath)
{
    std::ifstream temperatureFileStream(temperaturePath);

    if (!temperatureFileStream.is_open())
    {
        std::cout << "Couldn't open the temperature file \n";
        return 0;
    }

    std::string temperatureStr;

    temperatureFileStream >> temperatureStr;

    temperatureFileStream.close();

    int temperature = stoi(temperatureStr);

    // 54°C outputed as 54000
    return temperature;
}

std::tuple<int, int> getMemory(std::string memoryPath)
{
    std::ifstream memoryFileStream(memoryPath);

    if (!memoryFileStream.is_open())
    {
        std::cout << "Couldn't open the memory file" << "\n";
        return std::make_tuple(0, 0);
    }

    std::string rawTotal;
    std::string rawFree;

    getline(memoryFileStream, rawTotal); // MemTotal:        8255888 kB
    getline(memoryFileStream, rawFree);  // MemFree:         6814080 kB

    memoryFileStream.close();

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

std::string getHostname(std::string hostnamePath)
{
    std::ifstream hostnameFileStream(hostnamePath);

    std::string hostname;

    if (!hostnameFileStream.is_open())
    {
        std::cout << "Couldn't open the memory file" << "\n";
        return "Unknown";
    }

    getline(hostnameFileStream, hostname);

    hostnameFileStream.close();

    return hostname;
}

int main()
{
    // Adress to send data to (typically where your dashboard runs)
    std::string SERVER_ADDRESS = "localhost";

    // Files used as source
    std::string hostnamePath = "/etc/hostname";
    std::string memoryPath = "/proc/meminfo";
    std::string temperaturePath = "/sys/class/thermal/thermal_zone0/temp";

    std::cout << getTemperature(temperaturePath) << "\n";

    std::tuple<int, int> memoryData = getMemory(memoryPath);

    int totalMemory = std::get<0>(memoryData);
    int freeMemory = std::get<1>(memoryData);

    std::cout << totalMemory << "\n"
              << freeMemory << "\n";

    std::cout << getHostname(hostnamePath) << "\n";

    // TODO: add client socket that periodically sends system data to Python socket

    return 0;
}