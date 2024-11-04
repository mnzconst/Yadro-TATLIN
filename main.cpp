#include <iostream>
#include "TapeSorter.hpp"
#include "FileTape.hpp"

int main(int argc, char **argv) {
//    if (argc != 2) {
//        std::cerr << "ERROR: Unexpected amount of files" << std::endl;
//        return 1;
//    }

    TapeSorter<FileTape<true>, FileTape<false>> sorter{
        "/home/mnzconst/Desktop/mse/sem_2/stazh/Yadro-TATLIN/resources/input.txt",
        "/home/mnzconst/Desktop/mse/sem_2/stazh/Yadro-TATLIN/resources/output.txt"
    };
    sorter.mergeSort();
}
