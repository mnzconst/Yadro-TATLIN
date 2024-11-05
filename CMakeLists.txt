cmake_minimum_required(VERSION 3.20)
project(Yadro_TATLIN)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(${PROJECT_NAME}
        main.cpp
        src/Tape.cpp
)
