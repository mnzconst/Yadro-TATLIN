#ifndef YADRO_TATLIN_TAPESORTER_HPP
#define YADRO_TATLIN_TAPESORTER_HPP

#include "FileTape.hpp"
#include <memory>

template<typename InputTape, typename OutputTape>
class TapeSorter{
public:
    TapeSorter(const std::string &inputPath,
               const std::string &outputPath,
               char inputDelimiter = ',',
               char outputDelimiter = ',') :
        inputTape{std::make_unique<InputTape>(inputPath, inputDelimiter)},
        outputTape{std::make_unique<OutputTape>(outputPath, outputDelimiter)}
    {}

    void mergeSort()
    {
        std::cout << "size = " << inputTape->size() << std::endl;
        std::cout << "move forward" << std::endl;
        while(!inputTape->endOfTape()) {
            std::cout << inputTape->read() << std::endl;
            inputTape->moveRight();
        }
//
//        std::cout << "move forward" << std::endl;
//        inputTape->rewindBegin();
//        for(int i = 0; !inputTape->eof(); ++i) {
//            if (i % 2 == 1) {
//                inputTape->moveRight();
//            } else {
////                inputTape->read();
//                std::cout << inputTape->read() << std::endl;
//            }
//            inputTape->moveRight();
//        }

//        inputTape->rewindEnd();
//        std::cout << std::endl;
//        std::cout << "move backward" << std::endl;
//        while(!inputTape->bot()) {
//            std::cout << inputTape->read() << std::endl;
//            inputTape->moveLeft();
//        }
//        std::cout << inputTape->read() << std::endl;

        inputTape->rewindBegin();
        std::cout << "write" << std::endl;
        std::cout << inputTape->read() << std::endl;
//        inputTape->write(v + 10);
//        std::cout << inputTape->read() << std::endl;

//        while(!inputTape->endOfTape()) {
//            auto value = inputTape->read();
//            outputTape->write(value);
//            inputTape->moveRight();
//            outputTape->moveRight();
//        }

        outputTape->rewindBegin();
        while(!outputTape->endOfTape()) {
            auto v = outputTape->read();
            outputTape->write(v + 1000);
            outputTape->moveRight();
        }
    }

private:
    std::unique_ptr<AbstractTape<true>> inputTape;
    std::unique_ptr<AbstractTape<false>> outputTape;
};

#endif //YADRO_TATLIN_TAPESORTER_HPP
