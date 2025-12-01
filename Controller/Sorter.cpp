#include "Sorter.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

bool Sorter::SortAndAggregate(const std::string& temp_dir, const std::string& aggregated_file) {
    std::vector<std::string> lines;

    for (const auto& entry : fs::directory_iterator(temp_dir)) {
        if (!entry.is_regular_file()) continue;
        std::string filename = entry.path().filename().string();

        // Only pick intermediate files (mapper output)
        if (filename.find("intermediate_mapper") != std::string::npos) {
            std::ifstream in(entry.path(), std::ios::binary);
            if (!in.is_open()) {
                std::cerr << "Failed to open " << entry.path() << "\n";
                continue;
            }
            std::string line;
            while (std::getline(in, line)) {
                if (!line.empty()) lines.push_back(line);
            }
        }
    }

    if (lines.empty()) {
        std::cerr << "No lines read from intermediate files!\n";
        return false;
    }

    // Sort for consistency
    std::sort(lines.begin(), lines.end());

    std::ofstream out(aggregated_file, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to write aggregated file: " << aggregated_file << "\n";
        return false;
    }

    for (auto& line : lines) out << line << "\n";

    return true;
}
