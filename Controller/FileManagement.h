#pragma once
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class FileManagement
{
public:
    FileManagement(const std::string& input,
        const std::string& output,
        const std::string& temp);

    void initialize();
    std::vector<fs::path> inputFiles() const;
    std::ifstream openFile(const fs::path& filepath) const;

    // Open temp file in binary mode
    std::unique_ptr<std::ofstream> openTemp(const std::string& fileName) const;

    void writeFile() const;
    const fs::path& getTempDir() const { return tempDir; }

private:
    fs::path inputDir;
    fs::path outputDir;
    fs::path tempDir;
};
