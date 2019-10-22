#ifndef _MODULE_EDITOR_H_
#define _MODULE_EDITOR_H_

#include "Module.h"
#include <string>
#include <vector>

class Panel;
enum KEY_STATE;

enum Panel_Type {
	CONFIG,
	DEBUG,
	ABOUT,
	HIERARCHY,
	INSPECTOR,
	SCENE,
	TOTAL
};

class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();
	bool Init(const nlohmann::json::iterator& it);
	bool CleanUp();
	void Draw();
	bool Start();
	update_status Update(float dt);

	void LogInput(int key, KEY_STATE state, bool mouse = false);
	void LogDebug(const char* text);
	void LogFrame(float fps, float ms);

private:
	std::vector<Panel*> panels;
	void BeginDockSpace();
};

#endif // !_MODULE_EDITOR_H_



