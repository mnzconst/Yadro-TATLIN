#ifndef TAPESORTER_HPP
#define TAPESORTER_HPP

#include "FileTape.hpp"
#include <memory>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// TODO: написать коммент
template<typename InputTape, typename OutputTape>
class TapeSorter{
public:
    TapeSorter() = default;

    TapeSorter(const std::string &inputPath,
               const std::string &outputPath,
               char inputDelimiter = ',',
               char outputDelimiter = ',') :
        inputTape{std::make_shared<InputTape>(inputPath, inputDelimiter)},
        outputTape{std::make_shared<OutputTape>(outputPath, outputDelimiter)}
    {}

    TapeSorter(const TapeSorter &) = delete;
    TapeSorter &operator=(const TapeSorter &) = delete;

    TapeSorter(TapeSorter &&other) = default;
    TapeSorter &operator=(TapeSorter &&other) = default;

    // TODO: написать коммент, а если выходная нечистая?
    void mergeSort()
    {
        uint32_t n = inputTape->size();

        auto leftTape = std::make_shared<FileTape<false>>
                ("/home/mnzconst/Desktop/mse/sem_2/stazh/Yadro-TATLIN/resources/tmp/left.txt", ',');
        auto rightTape = std::make_shared<FileTape<false>>
                ("/home/mnzconst/Desktop/mse/sem_2/stazh/Yadro-TATLIN/resources/tmp/right.txt", ',');

        for (uint32_t width = 1; width < n; width *= 2) {
            inputTape->rewindBegin();
            outputTape->rewindBegin();

            for (uint32_t i = 0; i < n; i += 2 * width) {
                leftTape->rewindBegin();
                rightTape->rewindBegin();

                uint32_t leftSize = std::min(n, i + width) - i;
                uint32_t rightSize = 0;

                if (std::min(n, i + 2 * width) >= i + width) {
                    rightSize = std::min(n, i + 2 * width) - i - width;
                }

                copyTape(leftSize, inputTape, leftTape);
                copyTape(rightSize, inputTape, rightTape);

                leftTape->rewindBegin();
                rightTape->rewindBegin();

                merge(leftTape, rightTape, outputTape);
            }

            inputTape->rewindBegin();
            outputTape->rewindBegin();
            copyTape(n, outputTape, inputTape);
        }

        inputTape->rewindBegin();
        outputTape->rewindBegin();
        copyTape(n, inputTape, outputTape);
    }

    // TODO: мб добавить в FileTape инкапсуляцию создания и удаления
    void config(const fs::path &inputPath,
                const fs::path &outputPath,
                char inputDelimiter = ',',
                char outputDelimiter = ',',
                const fs::path &configPath = "")
    {
        inputTape = std::make_shared<InputTape>(inputPath, inputDelimiter);
        outputTape = std::make_shared<OutputTape>(outputPath, outputDelimiter);
        if (!configPath.empty()) {
            inputTape->config(configPath);
            outputTape->config(configPath);
        }
    }

private:
    std::shared_ptr<AbstractTape<false>> inputTape;
    std::shared_ptr<AbstractTape<false>> outputTape;

    static constexpr int BUFFER_SIZE = 8192 / sizeof(std::int32_t);
    static constexpr uint32_t M = 20;
    std::vector<int32_t> v{M};

     void merge(std::shared_ptr<AbstractTape<false>> leftTape,
                std::shared_ptr<AbstractTape<false>> rightTape,
                std::shared_ptr<AbstractTape<false>> resultTape)
    {
        uint32_t leftTapeSize = leftTape->size();
        uint32_t rightTapeSize = rightTape->size();
        uint32_t l = 0;
        uint32_t r = 0;

        while (l < leftTapeSize || r < rightTapeSize) {
            if (l == leftTapeSize) {
                copyTape(1, rightTape, resultTape);
                r++;
            } else if (r == rightTapeSize) {
                copyTape(1, leftTape, resultTape);
                l++;
            } else {
                std::int32_t firstVal = leftTape->read();
                std::int32_t secondVal = rightTape->read();

                if (firstVal < secondVal) {
                    copyTape(1, leftTape, resultTape);
                    l++;
                } else {
                    copyTape(1, rightTape, resultTape);
                    r++;
                }
            }
        }
    }

    void copyTape(uint32_t n,
                  const std::shared_ptr<AbstractTape<false>> &src,
                  const std::shared_ptr<AbstractTape<false>> &dst) {
        for (uint32_t i = 0; i < n; ++i) {
            dst->write(src->read());
            src->moveRight();
            dst->moveRight();
        }
    }

};

#endif // TAPESORTER_HPP
