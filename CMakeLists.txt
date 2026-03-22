cmake_minimum_required(VERSION 3.21)
project(PerfumeScraper)

set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp)

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(LibXml2 REQUIRED)

target_link_libraries(main PRIVATE
    cpr::cpr
    nlohmann_json::nlohmann_json
    LibXml2::LibXml2
)