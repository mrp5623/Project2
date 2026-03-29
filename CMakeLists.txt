cmake_minimum_required(VERSION 3.21)
project(PerfumeScraper)

set(CMAKE_CXX_STANDARD 17)

add_executable(main
        main.cpp
        FragranceHeap.cpp
)

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(main PRIVATE
    nlohmann_json::nlohmann_json
)