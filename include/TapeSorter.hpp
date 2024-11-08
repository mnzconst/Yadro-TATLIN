#ifndef TAPESORTER_HPP
#define TAPESORTER_HPP

#include "FileTape.hpp"
#include <memory>
#include <vector>
#include <algorithm>

// TODO: написать коммент
// TODO: добавить компаратор?
template<class InTape, class OutTape> // different types of tapes???
class TapeSorter{
    using InputTape = std::shared_ptr<AbstractTape<InTape::readOnlyV>>;
    using OutputTape = std::shared_ptr<AbstractTape<OutTape::readOnlyV>>;

public:
    TapeSorter(const InputTape &aInputTape,
               const OutputTape &aOutputTape) :
        inputTape{aInputTape},
        outputTape{aOutputTape},
        inputTapeCopy{std::make_shared<OutTape>()}
    {
        // Наверно мы не хотим изменять входную ленту
        copy(inputTape, inputTapeCopy, inputTape->size());
    }

    TapeSorter(const TapeSorter &) = delete;
    TapeSorter &operator=(const TapeSorter &) = delete;

    // TODO: надо или нет?
    TapeSorter(TapeSorter &&other) = default;
    TapeSorter &operator=(TapeSorter &&other) = default;

    // TODO: написать коммент, добавить оптимизации с вектором
    void mergeSort()
    {
        uint32_t inputSize = inputTape->size();
        auto leftTape = std::make_shared<OutTape>();
        auto rightTape = std::make_shared<OutTape>();
        auto mergeTape = std::make_shared<OutTape>();

        for (uint32_t i = 1; i < inputSize; i *= 2) {
            inputTapeCopy->rewind();
            mergeTape->rewind();

            for (uint32_t j = 0; j < inputSize - i; j += 2 * i) {
                leftTape->rewind();
                rightTape->rewind();

                uint32_t leftSize = i;
                uint32_t rightSize = std::min(inputSize, j + 2 * i) - (j + i);

                copy(inputTapeCopy, leftTape, leftSize);
                copy(inputTapeCopy, rightTape, rightSize);

                leftTape->rewind();
                rightTape->rewind();

                merge(leftTape, leftSize, rightTape, rightSize, mergeTape);
            }

            inputTapeCopy->rewind();
            mergeTape->rewind();
            copy(mergeTape, inputTapeCopy, mergeTape->size());
        }

        inputTapeCopy->rewind();
        outputTape->rewind();
        copy(inputTapeCopy, outputTape, inputSize);
    }

private:
    InputTape inputTape;
    OutputTape outputTape;
    OutputTape inputTapeCopy;

    static constexpr uint32_t M = 20;
    std::vector<int32_t> v;

     void merge(OutputTape leftTape, 
                uint32_t leftSize,
                OutputTape rightTape, 
                uint32_t rightSize,
                OutputTape resultTape)
    {
        uint32_t l = 0;
        uint32_t r = 0;

        while (l < leftSize || r < rightSize) {
            if (r == rightSize) {
                copy(leftTape, resultTape, 1);
                ++l;
            } else if (l == leftSize) {
                copy(rightTape, resultTape, 1);
                ++r;
            } else {
                auto leftValue = leftTape->read();
                auto rightValue = rightTape->read();
                if (leftValue < rightValue) {
                    copy(leftTape, resultTape, 1);
                    ++l;
                } else {
                    copy(rightTape, resultTape, 1);
                    ++r;
                }
            }
        }
    }

    template<class TapeType1, class TapeType2>
    void copy(const TapeType1 &aSrc, const TapeType2 &aDst, uint32_t aNum) {
        for (uint32_t i = 0; i < aNum; ++i) {
            aDst->write(aSrc->read());
            aSrc->moveRight();
            aDst->moveRight();
        }
    }

};

#endif // TAPESORTER_HPP
