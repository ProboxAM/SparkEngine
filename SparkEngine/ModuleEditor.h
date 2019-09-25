#pragma once
#include "Module.h"
#include <string>
#include "imgui.h"




class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();
	bool Init();
	bool CleanUp();
	update_status PostUpdate(float dt);
	update_status Update(float dt);

	void LogInput(int key, KEY_STATE state, bool mouse = false);
	void Log(const char* text);

private:

	std::string app_name = "";

	bool show_demo = false;
	bool show_config = false;
	bool show_debug = false;

	//Window Config
	int width = SCREEN_WIDTH * SCREEN_SIZE;
	int height = SCREEN_HEIGHT * SCREEN_SIZE;
	float brightness = 0;
	int refresh_rate = 0;
	bool window_settings[4];

	//Input window
	bool move_input_scroll = false;
	bool move_debug_scroll = false;
	ImGuiTextBuffer input_buff;

	//Hardware window
	SDL_version compiled_version;
	std::string GetCpuInfo();

	ImGuiTextBuffer debug_buff;
};

