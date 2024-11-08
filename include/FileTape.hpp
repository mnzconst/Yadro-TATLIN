#ifndef FILETAPE_HPP
#define FILETAPE_HPP

#include "AbstractTape.hpp"
#include "Utility.hpp"
#include <fstream>
#include <filesystem>
#include <utility>

// TODO: удалить в конце
#include <iostream>

namespace fs = std::filesystem;

// TODO: написать коммент
template<bool readOnly = true>
class FileTape : public AbstractTape<readOnly> {

    using Config = typename AbstractTape<readOnly>::Config;
    static constexpr uint16_t kElemMaxLen = 11; // 1 sign + 10 digits

    static constexpr uint16_t kMaxTmpFileNameLen = 12;

    // TODO: делать или нет?
    using T = int32_t;
    struct Iterator{
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
    };

public:

    static constexpr bool readOnlyV = readOnly;

    FileTape() :
        aDelimiter{','},
        aConfig{0, 0, 0},
        aSize{0},
        isCopied{false},
        isTmp{true},
        isCellRead{false},
        currentElement{0},
        currentCell{0}
    {
        auto fileName = "/" + details::genRandomString(kMaxTmpFileNameLen) + ".txt";
        filePath = details::createTmpFile(fs::current_path().string() + fileName);
        fileStream.open(filePath, std::ios::in | std::ios::out);

        fileBegin = fileStream.tellg();
        fileStream.seekp(0, std::ios::end);
        fileEnd = fileStream.tellp();
        fileStream.seekp(0, std::ios::beg);
    }

    explicit FileTape(const fs::path &aPath, char aDel = ',') :
        aDelimiter{aDel},
        filePath{aPath},
        aConfig{0, 0, 0},
        isCopied{false},
        isTmp{false},
        isCellRead{false},
        currentElement{0},
        currentCell{0}
    {
        if (readOnly) {
            fileStream.open(filePath, std::ios::in);
        } else {
            // В случае, когда мы хотим изменять элементы в файле, нам может не хватить места между двумя разделителями,
            // чтобы записать новое число, либо наоборот, число будет короче, и мы получим непонятно что.
            // Поэтому, чтобы не перетереть соседние ячейки, запишем все значения исходного файла во временный,
            // выделяя необходимое место для каждого числа, равное максимальной длине int32 + знак.
            // По окончании всех манипуляций, красиво перепишем новые значения в исходный файл.
            if (fs::file_size(filePath) == 0) {
                fileStream.open(filePath, std::ios::in | std::ios::out);
            } else {
                tmpCopyFile = details::createTmpFile(filePath);
                FileTape<true> originalTape(filePath, aDelimiter);
                copyTape(originalTape, tmpCopyFile, originalTape.size(), false);
                isCopied = true;
            }
        }
        if (!fileStream) {
            throw std::runtime_error("ERROR: Can't open the file: " + aPath.string());
        }

        fileBegin = fileStream.tellg();
        fileStream.seekp(0, std::ios::end);
        fileEnd = fileStream.tellp();
        fileStream.seekp(0, std::ios::beg);

        aSize = countElems();
    }

    FileTape(const fs::path &aPath, const fs::path &aConfig, char aDel = ',') :
        FileTape{aPath, aDel}
    {
        config(aConfig);
    }

    // TODO: проверить норм работает или нет
    FileTape(FileTape &&other) noexcept = default;
    FileTape &operator=(FileTape &&other) noexcept = default;

    // Перед операцией указатель стоит на первом элементе текущего числа
    // После операции указатель стоит на первом элементе следующего числа
    void moveRight() override
    {
        getNumber();
        isCellRead = false;
        ++currentCell;
    }

    // Перед операцией указатель стоит на первом элементе текущего числа
    // После операции указатель стоит на первом элементе предыдущего числа
    void moveLeft() override
    {
        if (beginOfTape()) {
            return;
        }
        fileStream.seekp(-2, std::ios::cur); // перемещаемся на последний символ предыдущего числа
        getNumberReversed();
        isCellRead = false;
        --currentCell;
    }

    void rewind() override
    {
        fileStream.seekp(0);
        currentCell = 0;
    }

    // Перед операцией указатель стоит на первом элементе текущего числа
    // После операции указатель стоит на первом элементе текущего числа
    int32_t read() override
    {
        if (!isCellRead) {
            uint32_t elementBegin = fileStream.tellp();
            std::string str = getNumber();
            if (str.empty()) {
                throw std::runtime_error("ERROR: Empty cell " + std::to_string(currentCell) + filePath.string());
            }
            fileStream.seekp(elementBegin);
            currentElement = std::stoi(str);
            isCellRead = true;
        }

        return currentElement;
    }

    // Перед операцией указатель стоит на первом элементе текущего числа
    // После операции указатель стоит на первом элементе текущего числа
    void write(int32_t aValue) override
    {
        if (readOnly) {
            throw std::runtime_error("ERROR: Readonly tape");
        }

        std::string value = std::to_string(aValue);
        auto valueLen = value.length();
        if (valueLen < kElemMaxLen) {
            if (aValue < 0) {
                value.insert(1, kElemMaxLen - valueLen, '0');
            } else {
                value.insert(0, kElemMaxLen - valueLen, '0');
            }
        }

        writeImpl(value, true);
        isCellRead = false;
        update();
    }

    uint32_t getCurrentCell() const
    {
        return currentCell;
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

    bool endOfTape() override
    {
        return fileStream.tellg() == fileEnd;
    }

    bool beginOfTape() override
    {
        return fileStream.tellg() == fileBegin;
    }

    uint32_t size() const override
    {
        return aSize;
    }

    ~FileTape() override
    {
        if (isCopied) {
            fileStream.close();
            FileTape<true> tmpTape(tmpCopyFile, aDelimiter);
            copyTape(tmpTape, filePath, tmpTape.size() - 1, true);
            writeImpl(std::to_string(tmpTape.read()), false);
            fs::remove(tmpCopyFile);
        }
        if (isTmp) {
            fs::remove(filePath);
        }
        fileStream.close();
    }

private:
    char aDelimiter;
    std::fstream fileStream;
    fs::path filePath;
    fs::path tmpCopyFile;
    Config aConfig;
    uint32_t aSize;
    uint32_t fileBegin;
    uint32_t fileEnd;
    bool isCopied;
    bool isTmp;
    bool isCellRead;
    int32_t currentElement;
    uint32_t currentCell;

    // Перед операцией указатель стоит на первом элементе текущего числа
    // После операции указатель стоит на первом элементе следующего числа
    std::string getNumber()
    {
        std::string res;
        char ch = 0;
        while (ch != aDelimiter && !endOfTape()) {
            fileStream >> ch;
            res += ch;
        }
        return res;
    }

    // Перед операцией указатель стоит на последнем элементе текущего числа
    // После операции указатель стоит на первом элементе текущего числа
    std::string getNumberReversed()
    {
        std::string res;
        char ch = 0;
        while (ch != aDelimiter && !beginOfTape()) {
            fileStream >> ch;
            res += ch;
            fileStream.seekp(-2, std::ios::cur);
        }
        if (ch == aDelimiter) {
            fileStream.seekp(2, std::ios::cur);
        }
        return res;
    }

    void writeImpl(const std::string &aValue, bool withDelimiter)
    {
        auto elementBegin = fileStream.tellp();
        fileStream.write(aValue.c_str(), static_cast<int32_t>(aValue.length()));
        if (withDelimiter) {
            fileStream << aDelimiter;
        }
        fileStream.seekp(elementBegin);
    }

    void update()
    {
        // update fileEnd
        auto prevFileEnd = fileEnd;
        auto currentPos = fileStream.tellp();
        fileStream.seekp(0, std::ios::end);
        fileEnd = fileStream.tellp();
        fileStream.seekp(currentPos);

        //update aSize
        if (prevFileEnd < fileEnd) {
            ++aSize;
        }
    }

    uint32_t countElems()
    {
        int res = 0;
        while (!endOfTape()) {
            moveRight();
            ++res;
        }
        rewind();
        return res;
    }

    void copyTape(FileTape<true> &src, const fs::path &dst, uint32_t aNum, bool clearNumbers)
    {
        fileStream.open(dst, std::ios::in | std::ios::out | std::ios::trunc);
        for (uint32_t i = 0; i < aNum; ++i) {
            if (clearNumbers) {
                writeImpl(std::to_string(src.read()), true);
            } else {
                write(src.read());
            }
            src.moveRight();
            moveRight();
        }
        fileStream.seekp(0);
    }

};

#endif // FILETAPE_HPP
