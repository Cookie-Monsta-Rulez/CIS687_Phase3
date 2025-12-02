Summary below of my new MapDLL files, which created a "MapDLL3.dll" project on my computer. 
These files are built from my Phase 2 Map files.  

Map3.h
	- abstract Map3 class, includes DLL export signatures for CreateMap and DestroyMap.
	- follows same logic as Map2.h for Phase 2, but supports multiple mapper/reducer processes.
	- mapID:  mapper process ID
	- reduceTotal: total number of reducer processes
	- interFile:  base name for intermediate files.

MapExport3.cpp
	- DLL export implementation
	- creates a MapTokenize3 object mapper instance
	- follows same logic as MapExport.cpp for Phase 2

MapTokenize3.h
	- header file for MapTokenize3 class
	- implements Map3 interface
	- builds on MapTokenize from Phase 2, but include two new variables, mapID = map process ID, 
	  reduceTotal = total number of reducer processes, buffer = vector of buffers (one / reducer)

MapTokenize3.cpp
	- methods for Map3
	- tokenize() = same as Phase 2
	- map() = hashes tokens to reducer partitions based upon reduceTotal
	- exportData() = appends data to correct memory buffer in vector
	- flush() = writes a single reducer partition buffer to disk
	- finalize() = flushes all buffers at end of map implementation
	- createFile() = creates a specific mapper/reducer filename (i.e "inter_m0_r0.txt")

FileManagement3.h
	- abstract interface for multiple intermediate files
	- files are opened on first write and kept open until all writes are complete.

FMAdapter3.h
	- adapter that implements FileManagement3 interface using Phase 1 FileManagement
	- vector used to keep track of tempfiles.  One per reducer.
	- reduceTotal parameter used to allocate vector size.
