#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
namespace fs = std::filesystem;

extern "C" __declspec(dllexport)
void ReduceFunction(const std::string& aggregated_file, const std::vector<int>& /*dummyVec*/) {
    std::ifstream input(aggregated_file);
    if (!input.is_open()) return;

    fs::path output_dir = fs::absolute(fs::path(aggregated_file).parent_path() / "../output");
    fs::create_directories(output_dir);
    fs::path output_file = output_dir / "final_results.txt";
    std::ofstream output(output_file);

    std::unordered_map<std::string, int> counts;
    std::string line, key;
    int value;
    while (std::getline(input, line)) {
        std::istringstream iss(line);
        if (iss >> key >> value) counts[key] += value;
    }

    for (auto& [k, v] : counts) output << k << "\t" << v << "\n";
    std::ofstream(output_dir / "SUCCESS").close();
}
