#ifndef _META_FILE_H_
#define _META_FILE_H_

#include "Application.h"
#include <map>

class MetaFile {
public:
	MetaFile();
	~MetaFile();

	bool loaded = false;

	int modification_date;
	std::string file;
	std::string original_file;
	std::string exported_file;
	uint id;

	virtual void SetDefault() {}
	
};

#endif // !_META_FILE_H_



