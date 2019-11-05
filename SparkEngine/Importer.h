#ifndef _IMPORTER_H_
#define _IMPORTER_H_

#include "Globals.h"

class Importer
{
public:
	Importer();
	~Importer();

	virtual bool Init() { return true; }
	virtual bool CleanUp() { return true; }

	virtual bool Import(const char* file, std::string& output_file) { return true; }
	virtual bool Import(const void* buffer, uint size, std::string& output_file) { return true; }
};

#endif // !_IMPORTER_H_



