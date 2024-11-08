cmake_minimum_required(VERSION 3.20)
project(Yadro_TATLIN)

set(CMAKE_CXX_STANDARD 17)
set(-pedantic -Wall -Wcast-align -Wcast-qual -Wconversion -Wdouble-promotion -Weffc++ -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Woverloaded-virtual -Wshadow -Wstrict-aliasing -Wuninitialized -Wvla)

include_directories(include)

add_executable(${PROJECT_NAME}
        main.cpp
        src/Tape.cpp
)
