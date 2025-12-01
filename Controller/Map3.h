/////////////////////////////
// David Zapata
// Professor Scott Roueche
// CSE 687 - Object Oriented Design
// Syracuse University
// Phase 3 - MapReduce
// November 19, 2025
// Map3.h file - Header file to define the Map interface

#pragma once
#include <cstddef>
#include <string>

class FileManagement3;

class Map3
{
public:

	virtual void map(const std::string& key, const std::string& value) = 0;
	virtual void finalize() = 0;

	virtual ~Map3() {}

}; // end class Map3

extern "C"
{
	using CreateMapFunc = Map3 * (__stdcall*)(FileManagement3* fm, const char* interFile, int mapID, int reduceTotal, std::size_t flushbytes);
	using DestroyMapFunc = void(__stdcall*)(Map3* instance);

} // end extern
