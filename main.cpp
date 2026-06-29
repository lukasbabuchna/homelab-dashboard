#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <tuple>
#include <regex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <nlohmann/json.hpp>

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

std::tuple<std::string, std::string> getMemory(std::string memoryPath)
{
    std::ifstream memoryFileStream(memoryPath);

    if (!memoryFileStream.is_open())
    {
        std::cout << "Couldn't open the memory file" << "\n";
        return std::make_tuple("0", "0");
    }

    std::string rawTotal;
    std::string rawFree;

    getline(memoryFileStream, rawTotal); // MemTotal:        8255888 kB
    getline(memoryFileStream, rawFree);  // MemFree:         6814080 kB

    memoryFileStream.close();

    std::smatch match;
    std::regex regex(R"(\d+)");

    std::string totalMem;
    std::string freeMem;

    // Regex on total

    if (std::regex_search(rawTotal, match, regex))
    {
        totalMem = match.str();
    }
    else
    {
        std::cout << "Could find number in total memory string" << "\n";
        totalMem = "0";
    }

    // Regex on free

    if (std::regex_search(rawFree, match, regex))
    {
        freeMem = match.str();
    }
    else
    {
        std::cout << "Could find number in total memory string" << "\n";
        freeMem = "0";
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

    std::tuple<std::string, std::string> memoryData = getMemory(memoryPath);

    std::string totalMemory = std::get<0>(memoryData);
    std::string freeMemory = std::get<1>(memoryData);

    std::cout << totalMemory << "\n"
              << freeMemory << "\n";

    std::cout << getHostname(hostnamePath) << "\n";

    // TODO: add client socket that periodically sends system data to Python socket

    // seting up socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8181);
    serverAddress.sin_addr.s_addr = inet_addr("192.168.88.42");

    while (true)
    {
        connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

        // send(clientSocket, message, strlen(message), 0);
        // TODO: add JSON
    }

    return 0;
}