#ifndef _PANEL_CONFIG_H_
#define _PANEL_CONFIG_H_

#include "Panel.h"

enum KEY_STATE;

class PanelConfiguration : public Panel
{

	struct GPUinfo {
		unsigned int vendor_id;
		unsigned int device_id;
		std::string gfx_brand;
		float memory_budget;
		float  memory_usage;
		float memory_available;
		float memory_reserved;
	};

public:
	PanelConfiguration(bool a);
	~PanelConfiguration();

	void Draw();
	void Start();

	void LogInput(int key, KEY_STATE state, bool mouse = false);
	void LogFrame(float fps, float ms);

private:
	//Application window
	std::vector<float> fps_log;
	std::vector<float> ms_log;

	//Input window
	bool move_input_scroll = false;
	ImGuiTextBuffer input_buff;

	//Hardware window
	SDL_version compiled_version;
	std::string GetCpuInfo();
	GPUinfo gpu_info;

	void UpdateGpuInfo();
};

#endif // !_PANEL_CONFIG_H_

