#ifndef FILETAPEIMPL_HPP
#define FILETAPEIMPL_HPP

#include "AbstractTape.hpp"
#include <string>
#include <fstream>
#include <filesystem>
#include <utility>

template<bool readOnly = false>
class FileTape : public AbstractTape<readOnly> {
    struct Config{
        uint32_t delayReadWrite;
        uint32_t delayRewind;
        uint32_t delayShift;
    };

    static constexpr uint16_t kElemMaxLen = 11; // 1 sign + 10 digits

public:

    /*
     *
     */
    explicit FileTape(const std::string &aPath, char aDelimiter = ',') :
        delimiter{aDelimiter},
        aConfig{0, 0, 0},
        isCellRead{false},
        currentElement{0}
    {
        if (readOnly) {
            fileStream.open(aPath, std::ios::in);
        } else {

            fileStream.open(aPath, std::ios::in | std::ios::out);
        }
        if (!fileStream) {
            std::cerr << "ERROR: Can't open the file: " << aPath << std::endl;
        }

        aSize = std::filesystem::file_size(aPath);
        fileBegin = fileStream.tellg();
        fileStream.seekp(0, std::ios::end);
        fileEnd = fileStream.tellp();
        fileStream.seekp(0, std::ios::beg);
    }

    /*
     *
     */
    FileTape(const FileTape &) = delete;
    FileTape &operator=(const FileTape &) = delete;

    /*
     *
     */
    FileTape(FileTape &&other) = default;
    FileTape &operator=(FileTape &&other) = default;

    /*
     * Перед операцией указатель может стоять либо на последнем символе предыдущего элемента,
     * либо на первом символе текущего элемента
     * После операции указатель стоит на первом элементе следующего числа
     * Выход за границу на стороне пользователя
     */
    void moveRight() override
    {
        getNumber();
        isCellRead = false;
    }

    /*
     * Перед операцией указатель может стоять либо на разделителе, либо на первом символе элемента
     * После операции указатель стоит на первом элементе предыдущего числа
     * Выход за границу на стороне пользователя
     */
    void moveLeft() override
    {
        if (beginOfTape()) {
            return;
        }
        fileStream.seekp(-2, std::ios::cur); // last symbol
        getNumberReversed();
        isCellRead = false;
    }

    void rewindBegin() override
    {
        fileStream.seekp(0);
    }

    // Головка смотрит на последний символ в элементе, переводит ее на первый символ в этом числе.
    std::string getNumberReversed()
    {
        std::string res;
        char ch = 0;
        while(ch != delimiter && !beginOfTape()) {
            fileStream >> ch;
            res += ch;
            fileStream.seekp(-2, std::ios::cur);
        }
        if (ch == delimiter) {
            fileStream.seekp(2, std::ios::cur);
        }
        return res;
    }

    void rewindEnd() override
    {
        fileStream.seekp(-1, std::ios::end);
//        skipDelimiters();
        getNumberReversed();
    }

    void skipDelimiters() {
        char ch = 0;
        while(ch == delimiter) {
            fileStream >> ch;
            fileStream.seekp(-2, std::ios::end);
        }
//        fileStream.seekp(-1, std::ios::end);
    }

    /*
     * Выводит число в ячейке, на которую сейчас указывает магнитная головка.
     * Перед началом операции указатель стоит на начале этого элемента.
     * После указатель стоит на начале этого элемента.
     */
    int32_t read() override
    {
//        if (!isCellRead) {
            std::string str = getNumber();
            fileStream.seekp(-static_cast<int32_t>(str.length()), std::ios::cur);
            if (str.empty()) {
                throw std::runtime_error("empty cell");
            }
            currentElement = std::stoi(str);
            isCellRead = true;
//        }
        return currentElement;
    }

    void write(int32_t aValue) override
    {
        if (readOnly) {
            throw std::runtime_error("read only file");
        }
        std::string str = std::to_string(aValue);

        if (str.length() < kElemMaxLen) {
            str.insert(0, kElemMaxLen - str.length(), '0');
        }

        for(char ch : str) {
            fileStream.put(ch);
        }
        fileStream.put(delimiter);
        fileStream.seekp(-static_cast<int32_t>(str.length()), std::ios::cur);
    }

    void writeDelimiter() {
        fileStream << delimiter;
    }

    void config(const std::string &aPath) override
    {
        std::fstream configFile{aPath};
        if (!configFile) {
            std::cerr << "ERROR: Can't open the file: " << aPath << std::endl;
        }

        // TODO: add protection
        configFile >> aConfig.delayReadWrite >> aConfig.delayRewind >> aConfig.delayShift;
    }

    bool endOfTape()
    {
        return fileStream.tellg() == fileEnd;
    }

    bool beginOfTape()
    {
        return fileStream.tellg() == fileBegin;
    }

    uint32_t size() const
    {
        return aSize;
    }

    ~FileTape() override
    {
        fileStream.close();
    }

private:
    std::fstream fileStream;
    char delimiter;
    uint32_t aSize;
    Config aConfig;

    uint32_t fileBegin;
    uint32_t fileEnd;

    bool isCellRead;
    int32_t currentElement;

    /*
     *
     */
    std::string getNumber()
    {
        std::string res;
        char ch = 0;
        while(ch != delimiter && !endOfTape()) {
//            std::cout << "pos = " << fileStream.tellg() << std::endl;
            fileStream >> ch;
            res += ch;
        }
        return res;
    }

};

#endif //FILETAPEIMPL_HPP
