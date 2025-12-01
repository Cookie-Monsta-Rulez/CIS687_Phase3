#include "MapTokenize3.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

void MapTokenize3::map(const std::string& key, const std::string& line)
{
    std::vector<std::string> tokens;
    tokenize(line, tokens);

    for (auto& t : tokens)
    {
        if (t.empty()) continue;
        std::size_t h = std::hash<std::string>{}(t);
        int reduceNum = static_cast<int>(h % reduceTotal);
        exportData(reduceNum, t, "1"); // "1" is count
    }
}

void MapTokenize3::tokenize(const std::string& line, std::vector<std::string>& out)
{
    out.clear();
    std::istringstream iss(line);
    std::string token;

    while (iss >> token)
    {
        // strip punctuation at start and end
        token.erase(token.begin(), std::find_if(token.begin(), token.end(),
            [](unsigned char c) { return std::isalnum(c); }));
        token.erase(std::find_if(token.rbegin(), token.rend(),
            [](unsigned char c) { return std::isalnum(c); }).base(),
            token.end());

        // convert to lowercase
        std::transform(token.begin(), token.end(), token.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (!token.empty())
            out.push_back(token);
    }
}

void MapTokenize3::exportData(int reduceNum, const std::string& key, const std::string& val) {
    if (key.empty()) return;

    auto& buff = buffer[reduceNum];
    buff.append(key);
    buff.push_back('\t');
    buff.append(val);
    buff.push_back('\n');
}



void MapTokenize3::flush(int reduceNum)
{
    if (!fm) return;
    auto& buff = buffer[reduceNum];
    if (buff.empty()) return;

    std::string fileName = createFile(reduceNum);
    if (!fm->openInter(fileName, reduceNum))
    {
        std::cerr << "[MapTokenize3] openInter failed: " << fileName << "\n";
        return;
    }

    // Write plain bytes
    if (!fm->appendInter(reduceNum, buff))
    {
        std::cerr << "[MapTokenize3] appendInter failed: " << fileName << "\n";
    }

    buff.clear();
}


void MapTokenize3::finalize()
{
    if (!fm) return;
    for (int r = 0; r < reduceTotal; ++r)
        flush(r);
    fm->closeInter();
}

std::string MapTokenize3::createFile(int reduceNum)
{
    std::ostringstream oss;
    oss << interName << "_m" << mapID << "_r" << reduceNum << ".txt";
    return oss.str();
}
