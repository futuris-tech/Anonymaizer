cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
project (Anonimaizer)
add_executable (Anonimaizer main.cpp)

configure_file (
	D:/build-Generator-Desktop_Qt_6_3_0_MinGW_64_bit-Debug/result.csv
    ${CMAKE_CURRENT_BINARY_DIR}/input.csv
    COPYONLY)
