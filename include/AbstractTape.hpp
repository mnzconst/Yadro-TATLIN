#ifndef ABSTRACTTAPE_HPP
#define ABSTRACTTAPE_HPP

#include <cstdint>

template<bool readOnly = false>
class AbstractTape {
public:
    virtual void moveRight() = 0;

    virtual void moveLeft() = 0;

    virtual int32_t read() = 0;

    virtual void write(int32_t aValue) = 0;

    virtual void config(const std::string &aPath) = 0;

    virtual ~AbstractTape() = default;

    // ?????
    virtual uint32_t size() const = 0;
    virtual bool endOfTape() = 0;
    virtual bool beginOfTape() = 0;
    virtual void rewindBegin() = 0;
    virtual void rewindEnd() = 0;
    virtual void writeDelimiter() = 0;
};

#endif //ABSTRACTTAPE_HPP
