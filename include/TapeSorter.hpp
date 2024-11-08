#ifndef TAPESORTER_HPP
#define TAPESORTER_HPP

#include "FileTape.hpp"
#include <memory>
#include <vector>

// TODO: написать коммент
template<class Tape1, class Tape2> // different types of tapes???
class TapeSorter{
    using InputTape = std::shared_ptr<AbstractTape<Tape1::readOnlyV>>;
    using OutputTape = std::shared_ptr<AbstractTape<Tape2::readOnlyV>>;

public:
    TapeSorter(const InputTape &aInputTape,
               const OutputTape &aOutputTape) :
        inputTape{aInputTape},
        outputTape{aOutputTape}
    {}

    TapeSorter(const TapeSorter &) = delete;
    TapeSorter &operator=(const TapeSorter &) = delete;

    // TODO: надо или нет?
    TapeSorter(TapeSorter &&other) = default;
    TapeSorter &operator=(TapeSorter &&other) = default;

    // TODO: написать коммент, а если разные типы тейпов
    void mergeSort()
    {
        uint32_t n = inputTape->size();

        auto leftTape = std::make_shared<Tape2>();
        auto rightTape = std::make_shared<Tape2>();

        for (uint32_t width = 1; width < n; width *= 2) {
            inputTape->rewind();
            outputTape->rewind();

            for (uint32_t i = 0; i < n; i += 2 * width) {
                leftTape->rewind();
                rightTape->rewind();

                uint32_t leftSize = std::min(n, i + width) - i;
                uint32_t rightSize = 0;

                if (std::min(n, i + 2 * width) >= i + width) {
                    rightSize = std::min(n, i + 2 * width) - i - width;
                }

                copyTape(leftSize, inputTape, leftTape);
                copyTape(rightSize, inputTape, rightTape);

                leftTape->rewind();
                rightTape->rewind();

                merge(leftTape, rightTape, outputTape);
            }

            inputTape->rewind();
            outputTape->rewind();
            copyTape(n, outputTape, inputTape);
        }

        inputTape->rewind();
        outputTape->rewind();
        copyTape(n, inputTape, outputTape);
    }

private:
    InputTape inputTape;
    OutputTape outputTape;

    static constexpr int BUFFER_SIZE = 8192 / sizeof(std::int32_t);
    static constexpr uint32_t M = 20;
    std::vector<int32_t> v{M};

     void merge(OutputTape leftTape,
                OutputTape rightTape,
                OutputTape resultTape)
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
                  const OutputTape &src,
                  const OutputTape &dst) {
        for (uint32_t i = 0; i < n; ++i) {
            dst->write(src->read());
            src->moveRight();
            dst->moveRight();
        }
    }

};

#endif // TAPESORTER_HPP
