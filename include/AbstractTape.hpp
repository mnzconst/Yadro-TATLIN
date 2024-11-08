#ifndef ABSTRACTTAPE_HPP
#define ABSTRACTTAPE_HPP

#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

template<bool readOnly = true>
class AbstractTape {
protected:
    AbstractTape() = default;

    struct Config{
        uint32_t delayReadWrite;
        uint32_t delayRewind;
        uint32_t delayShift;
    };
    
public:
    AbstractTape(const AbstractTape &) = delete;
    AbstractTape &operator=(const AbstractTape &) = delete;

    virtual void moveRight() = 0;

    virtual void moveLeft() = 0;

    virtual int32_t read() = 0;

    virtual void write(int32_t aValue) = 0;

    virtual void rewind() = 0;

    virtual void config(const fs::path &aPath) = 0;

    virtual uint32_t size() const = 0;

    virtual ~AbstractTape() = default;

    // TODO: спорные методы, учитывая, что мы поддерживаем size,
    // но для внутренней работы все равно нужны
    virtual bool endOfTape() = 0;

    virtual bool beginOfTape() = 0;

    // TODO: надо или нет?
    AbstractTape(AbstractTape &&other) noexcept = default;
    AbstractTape &operator=(AbstractTape &&other) noexcept = default;
};

#endif //ABSTRACTTAPE_HPP
