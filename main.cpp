#include "TapeSorter.hpp"
#include "FileTape.hpp"
#include "VectorTape.hpp"
#include <iostream>
#include <cstring>
#include <memory>

// TODO: подумать
#define INPUTREADONLY true
#define OUTPUTREADONLY false

int main(int argc, char **argv)
{
    // TODO: надо нет?
    std::cout << "Format: <input file> [delimiter] <output file> [delimiter] [config]\n"
              << "Delimiters must be length 1"
              << std::endl;
    
    // TODO: проверить все варианты
    if (argc < 3) {
        std::cerr << "ERROR: Too few arguments\n"
                  << "Format: <input file> [delimiter] <output file> [delimiter] [config]\n"
                  << "Delimiters must be length 1"
                  << std::endl;
        return 1;
    }

    if (argc > 6) {
        std::cerr << "ERROR: Too many arguments\n"
                  << "Format: <input file> [delimiter] <output file> [delimiter] [config]"
                  << "Delimiters must be length 1"
                  << std::endl;
        return 1;
    }

    // TODO: мб убрать разные разделители, а то ужасно выглядит
    std::shared_ptr<AbstractTape<INPUTREADONLY>> inputTape;
    std::shared_ptr<AbstractTape<OUTPUTREADONLY>> outputTape;
    if (argc == 3) {
        inputTape = std::make_shared<FileTape<INPUTREADONLY>>(argv[1]);
        outputTape = std::make_shared<FileTape<OUTPUTREADONLY>>(argv[2]);
    } else if (argc == 4) {
        if (strlen(argv[2]) == 1) {
            inputTape = std::make_shared<FileTape<INPUTREADONLY>>(argv[1], *argv[2]);
            outputTape = std::make_shared<FileTape<OUTPUTREADONLY>>(argv[3]);
        } else if (strlen(argv[3]) == 1) {
            inputTape = std::make_shared<FileTape<INPUTREADONLY>>(argv[1]);
            outputTape = std::make_shared<FileTape<OUTPUTREADONLY>>(argv[2], *argv[3]);
        } else {
            inputTape = std::make_shared<FileTape<INPUTREADONLY>>(argv[1], argv[3]);
            outputTape = std::make_shared<FileTape<OUTPUTREADONLY>>(argv[2], argv[3]);
        }
    } else if (argc == 5) {
        if (strlen(argv[2]) == 1 && strlen(argv[4]) == 1) {
            inputTape = std::make_shared<FileTape<INPUTREADONLY>>(argv[1], *argv[2]);
            outputTape = std::make_shared<FileTape<OUTPUTREADONLY>>(argv[3], *argv[4]);
        } else if (strlen(argv[2]) == 1) {
            inputTape = std::make_shared<FileTape<INPUTREADONLY>>(argv[1], argv[4], *argv[2]);
            outputTape = std::make_shared<FileTape<OUTPUTREADONLY>>(argv[3], argv[4]);
        } else if (strlen(argv[3]) == 1) {
            inputTape = std::make_shared<FileTape<INPUTREADONLY>>(argv[1], argv[4]);
            outputTape = std::make_shared<FileTape<OUTPUTREADONLY>>(argv[2], argv[4], *argv[3]);
        } else {
            std::cerr << "ERROR: Delimiters length must be 1" << std::endl;
            return 1;
        }
    } else {
        inputTape = std::make_shared<FileTape<INPUTREADONLY>>(argv[1], argv[5], *argv[2]);
        outputTape = std::make_shared<FileTape<OUTPUTREADONLY>>(argv[3], argv[5], *argv[4]);
    }

    TapeSorter<FileTape<INPUTREADONLY>, FileTape<OUTPUTREADONLY>> sorter{inputTape, outputTape};
    sorter.mergeSort();

    // Pricol
    inputTape->rewind();
    std::shared_ptr<AbstractTape<INPUTREADONLY>> vTapeI = std::make_shared<VectorTape<INPUTREADONLY>>();
    std::shared_ptr<AbstractTape<OUTPUTREADONLY>> vTapeO = std::make_shared<VectorTape<OUTPUTREADONLY>>();
    for (uint32_t i = 0; i < inputTape->size(); ++i) {
        vTapeI->write(inputTape->read());
        vTapeI->moveRight();
        inputTape->moveRight();
    }
    vTapeI->rewind();

    TapeSorter<VectorTape<INPUTREADONLY>, VectorTape<OUTPUTREADONLY>> sorter2{vTapeI, vTapeO};
    sorter2.mergeSort();
    vTapeO->rewind();
    for (uint32_t i = 0; i < vTapeO->size(); ++i) {
        std::cout << vTapeO->read() << " ";
        vTapeO->moveRight();
    }
    std::cout << std::endl;
}
 