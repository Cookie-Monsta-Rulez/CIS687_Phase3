#include "Map3.h"
#include "FileManagement3.h"
#include "MapTokenize3.h"
#include <iostream>

extern "C" {
    __declspec(dllexport) Map3* __stdcall CreateMap(FileManagement3* fm, const char* interFile,
        int mapID, int reduceTotal, size_t flushBytes) {
        return new MapTokenize3(fm, interFile, mapID, reduceTotal, flushBytes);
    }

    __declspec(dllexport) void __stdcall DestroyMap(Map3* instance) {
        delete instance;
    }
}
