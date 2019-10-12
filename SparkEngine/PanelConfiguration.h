#ifndef _PANEL_CONFIG_H_
#define _PANEL_CONFIG_H_

#include "Panel.h"
enum KEY_STATE;

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration(bool a);
	~PanelConfiguration();

	void Draw();

	void LogInput(int key, KEY_STATE state, bool mouse = false);
	void LogFrame(float fps, float ms);

private:
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
	ImGuiTextBuffer input_buff;

	//Hardware window
	SDL_version compiled_version;
	std::string GetCpuInfo();

	bool depth_test = false;
	bool cull_face = false;
	bool lighting = false;
	bool color_material = false;
	bool texture_2D = false;
	bool wireframe = false;
};

#endif // !_PANEL_CONFIG_H_

