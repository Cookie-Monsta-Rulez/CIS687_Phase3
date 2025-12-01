#include <windows.h>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <sstream>

#include "Map3.h"
#include "FileManagement3.h"
#include "FMAdapter3.h"
#include "FileManagement.h"

namespace fs = std::filesystem;

// DLL function pointers
typedef Map3* (__stdcall* CreateMapFunc)(FileManagement3*, const char*, int, int, size_t);
typedef void(__stdcall* DestroyMapFunc)(Map3*);
typedef void(__cdecl* ReduceFunc)(const std::string&, const std::vector<int>&);

int main(int argc, char* argv[])
{
    if (argc < 8) {
        std::cerr << "Usage: Controller.exe <input_dir> <temp_dir> <output_dir> "
            << "<map_dll> <reduce_dll> <num_mappers> <num_reducers>\n";
        return 1;
    }

    fs::path input_dir = argv[1];
    fs::path temp_dir = argv[2];
    fs::path output_dir = argv[3];
    std::string map_dll = argv[4];
    std::string reduce_dll = argv[5];
    int num_mappers = std::stoi(argv[6]);
    int num_reducers = std::stoi(argv[7]);

    fs::create_directories(temp_dir);
    fs::create_directories(output_dir);

    // Load DLLs
    HMODULE hMapDll = LoadLibraryA(map_dll.c_str());
    HMODULE hReduceDll = LoadLibraryA(reduce_dll.c_str());
    if (!hMapDll || !hReduceDll) {
        std::cerr << "Failed to load DLLs\n";
        return 1;
    }

    auto CreateMap = (CreateMapFunc)GetProcAddress(hMapDll, "CreateMap");
    auto DestroyMap = (DestroyMapFunc)GetProcAddress(hMapDll, "DestroyMap");
    auto Reduce = (ReduceFunc)GetProcAddress(hReduceDll, "ReduceFunction");
    if (!CreateMap || !DestroyMap || !Reduce) {
        std::cerr << "Failed to get DLL function pointers\n";
        return 1;
    }

    // Collect input files
    std::vector<fs::path> input_files;
    for (auto& e : fs::directory_iterator(input_dir))
        if (e.is_regular_file()) input_files.push_back(e.path());

    // Create mapper instances with adapters
    FileManagement fm(input_dir.string(), output_dir.string(), temp_dir.string());
    std::vector<Map3*> mappers;
    std::vector<FMAdapter3*> adapters;

    for (int i = 0; i < num_mappers; ++i) {
        auto adapter = new FMAdapter3(fm, num_reducers);
        adapters.push_back(adapter);

        // Mapper-specific temp file prefix
        std::string tempPrefix = "intermediate_mapper_" + std::to_string(i);
        mappers.push_back(CreateMap(adapter, tempPrefix.c_str(), i, num_reducers, 1024));
    }

    // Distribute input lines across mappers (round-robin)
    int mapIndex = 0;
    for (auto& file : input_files) {
        std::ifstream fin(file);
        if (!fin.is_open()) continue;

        std::string line;
        int lineID = 0;
        while (std::getline(fin, line)) {
            mappers[mapIndex % num_mappers]->map(std::to_string(lineID), line);
            ++lineID;
        }
        ++mapIndex;
    }

    // Cleanup mappers (flush & close intermediate files)
    for (auto mapper : mappers) DestroyMap(mapper);
    for (auto adapter : adapters) delete adapter;

    // --- Aggregate intermediate files per reducer ---
    for (int r = 0; r < num_reducers; ++r) {
        fs::path agg_file = temp_dir / ("aggregated_r" + std::to_string(r) + ".txt");
        std::ofstream out(agg_file, std::ios::binary | std::ios::trunc);
        if (!out.is_open()) continue;

        for (auto& e : fs::directory_iterator(temp_dir)) {
            std::string fname = e.path().filename().string();

            // Match intermediate files exactly for this reducer
            if (fname.find("_r" + std::to_string(r) + ".txt") != std::string::npos &&
                fname.find("aggregated") == std::string::npos) {
                std::ifstream in(e.path(), std::ios::binary);
                if (!in.is_open()) continue;

                out << in.rdbuf();
            }
        }
        out.flush();
    }

    // --- Combine aggregated_r*.txt into final_results.txt ---
    std::unordered_map<std::string, int> finalCounts;

    for (int r = 0; r < num_reducers; ++r) {
        fs::path agg_file = temp_dir / ("aggregated_r" + std::to_string(r) + ".txt");
        std::ifstream in(agg_file);
        if (!in.is_open()) continue;

        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string key;
            int val;
            if (!(iss >> key >> val)) continue;
            finalCounts[key] += val;
        }
    }

    // Write final_results.txt
    fs::create_directories(output_dir);
    std::ofstream finalOut(output_dir / "final_results.txt", std::ios::out | std::ios::trunc);
    for (auto& [k, v] : finalCounts)
        finalOut << k << "\t" << v << "\n";

    // SUCCESS marker
    std::ofstream(output_dir / "SUCCESS").close();
    std::cout << "[Controller] MapReduce complete.\n";

    FreeLibrary(hMapDll);
    FreeLibrary(hReduceDll);

    return 0;
}
