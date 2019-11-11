#ifndef _MODULE_FILE_SYSTEM_H_
#define _MODULE_FILE_SYSTEM_H_

#include "Module.h"
#include <vector>

struct SDL_RWops;
int close_sdl_rwops(SDL_RWops *rw);

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(const char* game_path = nullptr);

	// Destructor
	~ModuleFileSystem();

	// Called before render is available
	bool Init(const nlohmann::json::iterator& it);

	// Called before quitting
	bool CleanUp();

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const;
	bool IsDirectory(const char* file) const;
	void CreateDirectory(const char* directory);
	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;
	bool CopyFromOutsideFS(const char* full_path, const char* destination);
	bool Copy(const char* source, const char* destination);
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;
	void NormalizePath(char* full_path) const;
	void NormalizePath(std::string& full_path) const;
	bool HasExtension(const char* path, std::string extension);
	void GetFilesFiltered(const char* directory, std::vector<std::string> & file_list, std::string filter);
	void GetFilesOfExtension(const char* directory, std::vector<std::string> & file_list, std::string extension);

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);

	const char* GetBasePath() const;
	const char* GetWritePath() const;
	const char* GetReadPaths() const;
};

#endif // !_MODULE_FILE_SYSTEM_H_
