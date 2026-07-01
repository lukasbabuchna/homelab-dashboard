#include <iostream>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <tuple>
#include <regex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

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
        totalMem = stoi(match.str());
    }
    else
    {
        std::cout << "Could find number in total memory string" << "\n";
        totalMem = 0;
    }

    // Regex on free

    if (std::regex_search(rawFree, match, regex))
    {
        freeMem = stoi(match.str());
    }
    else
    {
        std::cout << "Could find number in total memory string" << "\n";
        freeMem = 0;
    }

    // total memory , free memory

    return std::make_tuple(totalMem, freeMem); // values in kB
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

std::tuple<float, float, float> getLoadAvarage(std::string loadAvaragePath)
{
    std::ifstream loadAvarageFileStream(loadAvaragePath);

    if (!loadAvarageFileStream.is_open())
    {
        std::cout << "Couldn't open the load avarge file";
        return std::make_tuple(0, 0, 0);
    }

    std::string rawLoadAvg;

    getline(loadAvarageFileStream, rawLoadAvg); // 0.33 0.63 0.64 2/1673 16184

    // tokenizing the output
    std::vector<std::string> loadAvgTokens;
    std::string temp;

    rawLoadAvg = rawLoadAvg + " "; // the last token will be pushed to the vector

    for (int i = 0; i < rawLoadAvg.size(); i++)
    {
        if (rawLoadAvg[i] == ' ')
        {
            loadAvgTokens.push_back(temp);
            temp = "";
            continue;
        }

        temp = temp + rawLoadAvg[i];
    }

    float load1 = std::stof(loadAvgTokens[0]);  // load in 1 minute
    float load5 = std::stof(loadAvgTokens[1]);  // load in 5 minutes
    float load15 = std::stof(loadAvgTokens[2]); // load in 15 minutes

    return std::make_tuple(load1, load5, load15);
}

int getNumOfCPUCores(std::string numOfCPUCoresPath)
{
    std::ifstream numOfCPUCoresFileStream(numOfCPUCoresPath);

    std::string rawCoresRange;

    getline(numOfCPUCoresFileStream, rawCoresRange); // 0-5

    // tokenizing raw output to get number of cores: [0, 5]

    rawCoresRange = rawCoresRange + "-";
    std::vector<int> coreRangeTokenized;
    std::string temp;

    for (int i = 0; i < rawCoresRange.size(); i++)
    {
        if (rawCoresRange[i] == '-')
        {
            coreRangeTokenized.push_back(std::stoi(temp));
            temp = "";
            continue;
        }

        temp = temp + rawCoresRange[i];
    }

    int numOfCores = (coreRangeTokenized[1] - coreRangeTokenized[0]) + 1;

    return numOfCores;
}

int main()
{
    // Adress to send data to (typically where your web dashboard runs)
    std::string SERVER_ADDRESS = "127.0.0.1";
    int REFRESH_TIME = 1; // in seconds

    // Files used as source
    std::string hostnamePath = "/etc/hostname";
    std::string memoryPath = "/proc/meminfo";
    std::string temperaturePath = "/sys/class/thermal/thermal_zone0/temp";
    std::string loadAvaragePath = "/proc/loadavg";
    std::string numOfCPUCoresPath = "/sys/devices/system/cpu/online";

    // seting up socket

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8181);
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS.c_str());

    while (true)
    {
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

        connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

        // constructing JSON

        std::tuple<int, int> memoryData = getMemory(memoryPath);
        int totalMemory = std::get<0>(memoryData);
        int freeMemory = std::get<1>(memoryData);

        int temperature = getTemperature(temperaturePath);

        std::string hostname = getHostname(hostnamePath);

        std::tuple<float, float, float> avgLoads = getLoadAvarage(loadAvaragePath);
        float load1 = std::get<0>(avgLoads);
        float load5 = std::get<0>(avgLoads);
        float load15 = std::get<0>(avgLoads);

        int numOfCPUCores = getNumOfCPUCores(numOfCPUCoresPath);

        nlohmann::json jsonData = {{"temperature", temperature},
                                   {"totalMemory", totalMemory},
                                   {"freeMemory", freeMemory},
                                   {"hostname", hostname},
                                   {"load1", load1},
                                   {"load5", load5},
                                   {"load15", load15},
                                   {"numOfCPUCores", numOfCPUCores}};

        std::string jsonDataSerializated = jsonData.dump(); // turning JSON into string

        send(clientSocket, jsonDataSerializated.c_str(), strlen(jsonDataSerializated.c_str()), 0);

        close(clientSocket);

        sleep(REFRESH_TIME);
    }

    return 0;
}