#pragma once
#include "FileManagement3.h"
#include "FileManagement.h"
#include <vector>
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

        if (interFiles[reduceNum].is_open()) return true;

        interFiles[reduceNum] = fm.openTemp(file);
        return interFiles[reduceNum].is_open();
    }

    bool appendInter(int reduceNum, const std::string& s) override
    {
        if (reduceNum < 0 || reduceNum >= reduceTotal) return false;
        if (!interFiles[reduceNum].is_open()) return false;

        interFiles[reduceNum] << s;      // << ensures text mode
        interFiles[reduceNum].flush();    // flush after every write
        return static_cast<bool>(interFiles[reduceNum]);
    }

    void closeInter() override
    {
        for (auto& f : interFiles)
        {
            if (f.is_open())
            {
                f.flush();
                f.close();
            }
        }
    }

private:
    FileManagement& fm;
    int reduceTotal;
    std::vector<std::ofstream> interFiles;
};
