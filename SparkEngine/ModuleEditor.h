#pragma once
#include "Module.h"
#include <string>
#include <vector>
#include "imgui.h"

#define MAX_LOG 50

class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();
	bool Init(nlohmann::json::iterator it);
	bool CleanUp();
	void Draw();
	update_status Update(float dt);

	void LogInput(int key, KEY_STATE state, bool mouse = false);
	void LogDebug(const char* text);
	void LogFrame(float fps, float ms);

private:

	bool show_demo = false;
	bool show_config = false;
	bool show_debug = false;
	bool show_about = false;

	//Application window
	std::string app_name = "";
	std::string org_name = "";
	int max_fps = 0;
	std::vector<float> fps_log;
	std::vector<float> ms_log;

	//Window Config
	int width;
	int height;
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

	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;
	bool texture_2D = false;
};

