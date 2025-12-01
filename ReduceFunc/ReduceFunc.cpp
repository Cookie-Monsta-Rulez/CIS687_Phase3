#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>
namespace fs = std::filesystem;

extern "C" __declspec(dllexport)
void ReduceFunction(const std::string& aggregated_file, const std::vector<int>& /*dummy*/) {
    std::ifstream input(aggregated_file, std::ios::binary);
    if (!input.is_open()) {
        std::cerr << "ReduceFunction: Failed to open " << aggregated_file << "\n";
        return;
    }

    std::unordered_map<std::string, int> counts;
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream iss(line);
        std::string key;
        int val;
        if (!(iss >> key >> val)) continue;
        counts[key] += val;
    }

    // Output directory
    fs::path output_dir = fs::absolute(fs::path(aggregated_file).parent_path() / "../output");
    fs::create_directories(output_dir);

    std::ofstream out(output_dir / "final_results.txt", std::ios::trunc);
    for (auto& [k, v] : counts) out << k << "\t" << v << "\n";

    // SUCCESS marker
    std::ofstream(output_dir / "SUCCESS").close();
}
