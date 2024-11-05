#ifndef VECTORTAPE_HPP
#define VECTORTAPE_HPP

#include "AbstractTape.hpp"
#include <vector>

// Для демонстарции работы класса TapeSorter
// TODO: добавить оставшиеся методы AbstractTape

template<bool readOnly>
class VectorTape : AbstractTape<readOnly>{

    using Config = typename AbstractTape<readOnly>::Config;

public:
    VectorTape(const std::vector &aInput) :
        aData{aInput},
        currentPos{0}
    {}

    void moveRight()
    {
        ++currentPos;
    }

    void moveLeft()
    {
        --currentPos;
    }
    
    int32_t read()
    {
        return aData[currentPos];
    }
    
    void write(int32_t aValue)
    {
        aData[currentPos] = aValue;
    }
    
    void config(const std::string &aPath)
    {
        std::fstream configFile{aPath};
        if (!configFile) {
            std::cerr << "ERROR: Can't open the file: " << aPath << std::endl;
        }

        // TODO: add protection
        configFile >> aConfig.delayReadWrite >> aConfig.delayRewind >> aConfig.delayShift;
    }
    
    ~AbstractTape() = default;

private:
    std::vector<int32_t> aData;
    int32_t currentPos;
    Config config;
};

#endif // VECTORTAPE_HPP