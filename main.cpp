#include "TapeSorter.hpp"
#include "FileTape.hpp"
#include <iostream>
#include <cstring>

int main(int argc, char **argv)
{
    //TODO: проверить все варианты
    if (argc < 3) {
        std::cerr << "ERROR: Too few arguments\n"
                  << "Format: <input file> [delimiter] <output file> [delimiter] [config]"
                  << std::endl;
        return 1;
    }

    if (argc > 6) {
        std::cerr << "ERROR: Too many arguments\n"
                  << "Format: <input file> [delimiter] <output file> [delimiter] [config]"
                  << std::endl;
        return 1;
    }

    TapeSorter<FileTape<false>, FileTape<false>> sorter;
    if (argc == 3) {
        sorter.config(argv[1], argv[2]);
    } else if (argc == 4) {
        if (strlen(argv[2]) == 1) {
            sorter.config(argv[1], argv[3], *argv[2]);
        } else if (strlen(argv[3]) == 1) {
            sorter.config(argv[1], argv[2], ',', *argv[3]);
        } else {
            std::cerr << "ERROR: Expect argument format, delimiters length must be 1:\n"
                      << "Format: <input file> [delimiter] <output file> [delimiter] [config]"
                      << std::endl;
            return 1;
        }
    } else {
        if (strlen(argv[2]) != 1 || strlen(argv[4]) != 1) {
            std::cerr << "ERROR: Delimiters length must be 1" << std::endl;
            return 1;
        }
        if (argc == 5) {
            sorter.config(argv[1], argv[3], *argv[2], *argv[4]);
        } else {
            sorter.config(argv[1], argv[3], *argv[2], *argv[4], argv[5]);
        }
    }

    sorter.mergeSort();
}
 