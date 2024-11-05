#ifndef FILETAPE_HPP
#define FILETAPE_HPP

#include "AbstractTape.hpp"
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
    using T = int32_t;

    static constexpr uint16_t kElemMaxLen = 11; // 1 sign + 10 digits

    struct Iterator{
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
    };

public:
    FileTape(const fs::path &aPath, char aDelimiter) :
        delimiter{aDelimiter},
        aConfig{0, 0, 0},
        filePath{aPath},
        isCellRead{false},
        currentElement{0}
    {
        if (readOnly) {
            fileStream.open(aPath, std::ios::in);
        } else {
            // В случае, когда мы хотим изменять элементы в файле, нам может не хватить места между двумя разделителями,
            // чтобы записать новое число. Поэтому, чтобы не перетереть соседние ячейки, запишем все значения исходного
            // файла во временный, выделяя необходимое место для каждого числа, равное максимальной длине int32 + знак.
            // По окончании всех манипуляций, красиво перепишем новые значения в исходный файл.
            // Да, для клиента может быть неожиданностью, что одна операция записи может вылиться в О(n),
            // но мы рассчитываем на большое количество таких операций.
            if (fs::file_size(aPath) == 0) {
                fileStream.open(aPath, std::ios::in | std::ios::out);
            } else {
                copyFile(aPath);
            }
        }

        if (!fileStream) {
            throw std::runtime_error("ERROR: Can't open the file: " + aPath.string());
        }

        aSize = fs::file_size(aPath);

        // TODO: Оформить получше
        fileBegin = fileStream.tellg();
        fileStream.seekp(0, std::ios::end);
        fileEnd = fileStream.tellp();
        fileStream.seekp(0, std::ios::beg);
    }

    // TODO: проверить норм работает или нет
    FileTape(FileTape &&other) noexcept = default;
    FileTape &operator=(FileTape &&other) noexcept = default;

    // TODO: написать коммент
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

    // TODO: написать коммент
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
        fileStream.seekp(-2, std::ios::cur); // перемещаемся на последний символ предыдущего числа
        getNumberReversed();
        isCellRead = false;
    }

    void rewindBegin() override
    {
        fileStream.seekp(0);
    }

    // TODO: написать коммент
    /*
     * Выводит число в ячейке, на которую сейчас указывает магнитная головка.
     * Перед началом операции указатель стоит на начале этого элемента.
     * После указатель стоит на начале этого элемента.
     */
    int32_t read() override
    {
        if (!isCellRead) {
            auto elementBegin = fileStream.tellp();
            std::string str = getNumber();
            if (str.empty()) {
                throw std::runtime_error("ERROR: Empty cell");
            }
            fileStream.seekp(elementBegin);
            currentElement = std::stoi(str);
            isCellRead = true;
        }
        return currentElement;
    }

    // TODO: написать коммент
    void write(int32_t aValue) override
    {
        if (readOnly) {
            throw std::runtime_error("ERROR: Readonly tape");
        }
        auto elementBegin = fileStream.tellp();
        std::string str = std::to_string(aValue);
        if (str.length() < kElemMaxLen) {
            if (aValue < 0) {
                str.insert(1, kElemMaxLen - str.length(), '0');
            } else {
                str.insert(0, kElemMaxLen - str.length(), '0');
            }
        }
        fileStream.write(str.c_str(), kElemMaxLen);
        fileStream << delimiter;
        fileStream.seekp(elementBegin);
        isCellRead = false;
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
    fs::path filePath;

    uint32_t fileBegin;
    uint32_t fileEnd;

    bool isCellRead;
    int32_t currentElement;

    // TODO: написать коммент
    std::string getNumber()
    {
        std::string res;
        char ch = 0;
        while(ch != delimiter && !endOfTape()) {
            fileStream >> ch;
            res += ch;
        }
        if (endOfTape()) {
            std::cout << "end" << std::endl;
        }
        return res;
    }

    // TODO: написать коммент
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

    // TODO: написать коммент
    void copyFile(const fs::path &aPath) {
        FileTape<true> originalFile(aPath, delimiter);

        auto parent = aPath.parent_path();
        auto fileName = aPath.stem();
        auto extension = aPath.extension();
        auto tmpFileFile = parent.string() + "/tmp/" + fileName.string() + "_copy" + extension.string();

        if (!fs::exists(parent.string() + "/tmp")) {
            fs::create_directory(parent.string() + "/tmp");
        }

        fileStream.open(tmpFileFile);
        if (!fileStream.is_open()) {
            fileStream.open(tmpFileFile, std::ios::out);
            fileStream.close();
            fileStream.open(tmpFileFile, std::ios::in | std::ios::out);
        }

        while(!originalFile.endOfTape()) {
            write(originalFile.read());
            originalFile.moveRight();
            moveRight();
        }

        fileStream.seekp(0);
    }

};

#endif // FILETAPE_HPP
