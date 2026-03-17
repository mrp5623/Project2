cmake_minimum_required(VERSION 3.25)
project(Project2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(cpr CONFIG REQUIRED)
find_package(LibXml2 REQUIRED)

add_executable(Project2 main.cpp)

target_link_libraries(Project2 PRIVATE cpr::cpr LibXml2::LibXml2)