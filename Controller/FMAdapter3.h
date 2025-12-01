#pragma once
#include "FileManagement3.h"
#include "FileManagement.h"
#include <vector>
#include <memory>
#include <iostream>
#include <filesystem>

class FMAdapter3 : public FileManagement3
{
public:
    FMAdapter3(FileManagement& fm, int reduceTotal)
        : fm(fm), reduceTotal(reduceTotal)
    {
        interFiles.resize(reduceTotal);
    }

    bool openInter(const std::string& file, int reduceNum) override
    {
        if (reduceNum < 0 || reduceNum >= reduceTotal)
            return false;

        if (interFiles[reduceNum] && interFiles[reduceNum]->is_open())
            return true;

        interFiles[reduceNum] = fm.openTemp(file);
        if (interFiles[reduceNum] && interFiles[reduceNum]->is_open()) {
            std::cout << "[FMAdapter3] Opened file: " << file
                << " for reducer " << reduceNum << "\n";
            return true;
        }
        return false;
    }

    bool appendInter(int reduceNum, const char* data, std::size_t len) override
    {
        if (reduceNum < 0 || reduceNum >= reduceTotal) return false;
        if (!interFiles[reduceNum] || !interFiles[reduceNum]->is_open()) return false;

        interFiles[reduceNum]->write(data, static_cast<std::streamsize>(len));
        interFiles[reduceNum]->flush();
        return static_cast<bool>(*interFiles[reduceNum]);
    }

    void closeInter() override
    {
        for (int r = 0; r < reduceTotal; ++r)
        {
            if (interFiles[r] && interFiles[r]->is_open())
            {
                interFiles[r]->flush();
                interFiles[r]->close();
                std::cout << "[FMAdapter3] File closed for reducer #: " << r << "\n";
            }
        }
    }

private:
    FileManagement& fm;
    int reduceTotal;
    std::vector<std::unique_ptr<std::ofstream>> interFiles;
};
