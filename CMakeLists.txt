cmake_minimum_required(VERSION 3.16)
project(country_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(panstwa_serwer src/main.cpp src/serwer.cpp src/serwer.h src/lobby.cpp src/gracz.cpp )
target_link_libraries(panstwa_serwer PRIVATE )
    