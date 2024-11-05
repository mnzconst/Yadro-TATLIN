#ifndef TAPESORTER_HPP
#define TAPESORTER_HPP

#include "FileTape.hpp"
#include <memory>

template<typename InputTape, typename OutputTape>
class TapeSorter{
public:
    TapeSorter() = default;

    TapeSorter(const std::string &inputPath,
               const std::string &outputPath,
               char inputDelimiter = ',',
               char outputDelimiter = ',') :
        inputTape{std::make_unique<InputTape>(inputPath, inputDelimiter)},
        outputTape{std::make_unique<OutputTape>(outputPath, outputDelimiter)}
    {}

    TapeSorter(const TapeSorter &) = delete;
    TapeSorter &operator=(const TapeSorter &) = delete;

    TapeSorter(TapeSorter &&other) = default;
    TapeSorter &operator=(TapeSorter &&other) = default;

    void mergeSort()
    {
        // std::cout << "size = " << inputTape->size() << std::endl;
        // std::cout << "move forward" << std::endl;
        // while(!inputTape->endOfTape()) {
        //     std::cout << inputTape->read() << std::endl;
        //     inputTape->moveRight();
        // }
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

        // inputTape->rewindBegin();
        std::cout << "write" << std::endl;
        while(!inputTape->endOfTape()) {
            auto value = inputTape->read();
            outputTape->write(value);
            inputTape->moveRight();
            outputTape->moveRight();
        }

        // outputTape->rewindBegin();
        // while(!outputTape->endOfTape()) {
        //     auto v = outputTape->read();
        //     outputTape->write(v + 1000);
        //     outputTape->moveRight();
        // }
    }


    // TODO: мб добавить в FileTape инкапсуляцию создания и удаления
    void config(const fs::path &inputPath,
                const fs::path &outputPath,
                char inputDelimiter = ',',
                char outputDelimiter = ',',
                const fs::path &configPath = "")
    {
        inputTape = std::make_unique<InputTape>(inputPath, inputDelimiter);
        outputTape = std::make_unique<OutputTape>(outputPath, outputDelimiter);
        if (!configPath.empty()) {
            inputTape->config(configPath);
            outputTape->config(configPath);
        }
    }

private:
    std::unique_ptr<AbstractTape<true>> inputTape;
    std::unique_ptr<AbstractTape<false>> outputTape;
};

#endif // TAPESORTER_HPP
