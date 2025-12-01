#include "FileManagement.h"
#include <stdexcept>
#include <system_error>

FileManagement::FileManagement(const std::string& input,
    const std::string& output,
    const std::string& temp)
    : inputDir(fs::absolute(input)),
    outputDir(fs::absolute(output)),
    tempDir(fs::absolute(temp))
{
    // empty
}

void FileManagement::initialize()
{
    if (!fs::exists(inputDir) || !fs::is_directory(inputDir))
        throw std::runtime_error("Input directory does not exist: " + inputDir.string());

    std::error_code ec;
    fs::create_directories(outputDir, ec);
    if (ec) throw std::runtime_error("Could not create output dir: " + outputDir.string());

    ec.clear();
    fs::create_directories(tempDir, ec);
    if (ec) throw std::runtime_error("Could not create temp dir: " + tempDir.string());
}

std::vector<fs::path> FileManagement::inputFiles() const
{
    std::vector<fs::path> files;
    for (auto& entry : fs::directory_iterator(inputDir))
        if (entry.is_regular_file())
            files.push_back(entry.path());
    return files;
}

std::ifstream FileManagement::openFile(const fs::path& filepath) const
{
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (!in.is_open())
        throw std::runtime_error("Failed to open input file: " + filepath.string());
    return in;
}
std::ofstream FileManagement::openTemp(const std::string& fileName) const{
    fs::path path = tempDir / fileName;
    std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::app);
    if (!out.is_open())
        throw std::runtime_error("Failed to open temp file: " + path.string());
    return out;
}

void FileManagement::writeFile() const
{
    fs::path success = outputDir / "SUCCESS";
    std::ofstream out(success, std::ios::out | std::ios::trunc);
    if (!out.is_open())
        throw std::runtime_error("Failed to write SUCCESS file: " + success.string());
}
