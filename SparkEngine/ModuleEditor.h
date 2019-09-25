#pragma once
#include "Module.h"
#include <string>
#include "imgui.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();
	bool Init();
	bool CleanUp();
	update_status PostUpdate(float dt);
	update_status Update(float dt);

	void LogInput(int key, KEY_STATE state, bool mouse = false);

private:

	std::string app_name = "";

	bool show_demo = false;
	bool show_config = false;


	//Input window
	bool move_scroll = false;
	ImGuiTextBuffer input_buff;
};

