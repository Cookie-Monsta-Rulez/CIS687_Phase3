/////////////////////////////
// David Zapata
// Professor Scott Roueche
// CSE 687 - Object Oriented Design
// Syracuse University
// Phase 3 - MapReduce
// November 19, 2025
// FileManagement3.h file - Header file for interface for multiple temp files

#pragma once
#include <cstddef>
#include <string>

class FileManagement3
{
public:
	virtual ~FileManagement3() {}

	virtual bool openInter(const std::string& file, int reduceNum) = 0;

	virtual bool appendInter(int reduceNum, const char* data, std::size_t len) = 0;

	bool appendInter(int reduceNum, const std::string& s)
	{
		return appendInter(reduceNum, s.data(), s.size());
	}

	virtual void closeInter() = 0;

}; // end class FileManagement3