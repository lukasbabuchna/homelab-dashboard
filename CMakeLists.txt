cmake_minimum_required(VERSION 3.15)

project(homelab-dashboard CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.12.0
)

FetchContent_MakeAvailable(json)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)
