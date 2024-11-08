#ifndef VECTORTAPE_HPP
#define VECTORTAPE_HPP

#include "AbstractTape.hpp"
#include <fstream>
#include <vector>

// Особо не всматривайтесь, просто накидал для демонстрации работы класса TapeSorter
// TODO: добавить оставшиеся методы AbstractTape

template<bool readOnly>
class VectorTape : public AbstractTape<readOnly>{
    using Config = typename AbstractTape<readOnly>::Config;

public:
    static constexpr bool readOnlyV = readOnly;

    VectorTape() :
        currentPos{1}
    {}

    explicit VectorTape(const std::vector<int32_t> &aInput) :
        aData{aInput},
        currentPos{1}
    {}

    VectorTape(VectorTape &&other) noexcept = default;
    VectorTape &operator=(VectorTape &&other) noexcept = default;

    void moveRight() override
    {
        ++currentPos;
    }

    void moveLeft() override
    {
        --currentPos;
    }
    
    int32_t read() override
    {
        return aData[currentPos - 1];
    }
    
    void write(int32_t aValue) override
    {
        if (endOfTape()) {
            aData.push_back(aValue);
        } else {
            aData[currentPos - 1] = aValue;
        }
    }
    
    void rewind() override
    {
        currentPos = 1;
    }

    void config(const fs::path &aPath) override
    {
        std::fstream configFile{aPath};
        if (!configFile) {
            throw std::runtime_error("ERROR: Can't open config file: " + aPath.string());
        }

        // TODO: add protection
        configFile >> aConfig.delayReadWrite >> aConfig.delayRewind >> aConfig.delayShift;
    }
    
    uint32_t size() const override
    {
        return aData.size();
    }

    bool endOfTape() override
    {
        return currentPos == size() + 1;
    }

    bool beginOfTape() override
    {
        return currentPos == 0;
    }
    
    ~VectorTape() override = default;

private:
    std::vector<int32_t> aData;
    int32_t currentPos;
    Config aConfig;
};

#endif // VECTORTAPE_HPP