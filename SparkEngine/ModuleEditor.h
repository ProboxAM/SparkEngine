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

	//Window Config
	int width = SCREEN_WIDTH * SCREEN_SIZE;
	int height = SCREEN_HEIGHT * SCREEN_SIZE;
	float brightness = 0;
	int refresh_rate = 0;
	bool window_settings[4];
};

