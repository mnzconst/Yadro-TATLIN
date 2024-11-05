#ifndef ABSTRACTTAPE_HPP
#define ABSTRACTTAPE_HPP

#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

template<bool readOnly = false>
class AbstractTape {
protected:
    AbstractTape() = default;

    struct Config{
        uint32_t delayReadWrite;
        uint32_t delayRewind;
        uint32_t delayShift;
    };
    
public:
    virtual void moveRight() = 0;

    virtual void moveLeft() = 0;

    virtual int32_t read() = 0;

    virtual void write(int32_t aValue) = 0;

    virtual void config(const fs::path &aPath) = 0;

    virtual ~AbstractTape() = default;

    // TODO: решить, какие методы оставить
    AbstractTape(const AbstractTape &) = delete;
    AbstractTape &operator=(const AbstractTape &) = delete;
    virtual uint32_t size() const = 0;
    virtual bool endOfTape() = 0;
    virtual bool beginOfTape() = 0;
    virtual void rewindBegin() = 0;
};

#endif //ABSTRACTTAPE_HPP
