#include <filesystem>
#include <fstream>
#include <string>

namespace details {

    namespace fs = std::filesystem;

    // TODO: написать коммент
    static fs::path createTmpFile(const fs::path &aPath)
    {
        auto parent = aPath.parent_path().string();
        auto fileName = aPath.stem().string();
        auto extension = aPath.extension().string();
        auto tmpFileFile = parent + "/tmp/" + fileName + "_copy" + extension;

        if (!fs::exists(parent + "/tmp")) {
            fs::create_directory(parent + "/tmp");
        }

        std::fstream tmpFile(tmpFileFile);
        if (!tmpFile.is_open()) {
            tmpFile.open(tmpFileFile, std::ios::out);
            tmpFile.close();
        }

        return tmpFileFile;
    }

    static std::string genRandomString(uint32_t aLen)
    {
        std::string result;
        for (uint32_t i = 0; i < aLen; ++i) {
            int randomChar = rand() % (26 + 26 + 10);
            if (randomChar < 26)
                result += 'a' + randomChar;
            else if (randomChar < 26 + 26)
                result += 'A' + randomChar - 26;
            else
                result += '0' + randomChar - 26 - 26;
        }
        return result;
    }

    static void clearFile(const fs::path &aPath)
    {
        std::ofstream tmpStream;
        tmpStream.open(aPath, std::ofstream::out | std::ofstream::trunc);
        tmpStream.close();
    }

} // namespace details

