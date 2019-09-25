#pragma once
#include "Module.h"
#include <string>

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();
	bool Init();
	bool CleanUp();
	update_status PostUpdate(float dt);
	update_status Update(float dt);

private:

	std::string app_name = "";

	bool show_demo = false;
	bool show_config = false;
};

