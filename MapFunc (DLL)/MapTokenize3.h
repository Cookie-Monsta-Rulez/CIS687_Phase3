#pragma once
#include "Map3.h"
#include "FileManagement3.h"
#include <string>
#include <vector>
#include <iostream>

class MapTokenize3 : public Map3
{
public:
    MapTokenize3(FileManagement3* fm, const std::string& interFile,
        int mapID, int reduceTotal, std::size_t flushBytes)
        : fm(fm), interName(interFile), mapID(mapID),
        reduceTotal(reduceTotal), flushBytes(flushBytes),
        buffer(static_cast<size_t>(reduceTotal))
    {
        if (flushBytes == 0) flushBytes = 1;
        std::cout << "[MapTokenize3] Constructor: Mapper " << mapID
            << ", Reducers: " << reduceTotal
            << ", flushBytes: " << flushBytes << "\n";
    }

    ~MapTokenize3() { finalize(); }

    void map(const std::string& key, const std::string& line) override;
    void finalize() override;

private:
    FileManagement3* fm;
    std::string interName;
    int mapID;
    int reduceTotal;
    std::size_t flushBytes;
    std::vector<std::string> buffer;

    void flush(int reduceNum);
    void exportData(int reduceNum, const std::string& key, const std::string& value);
    void tokenize(const std::string& line, std::vector<std::string>& out);
    std::string createFile(int reduceNum);
};
