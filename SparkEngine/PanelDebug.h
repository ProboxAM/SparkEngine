#pragma once
#include "Panel.h"
class PanelDebug :
	public Panel
{
public:
	PanelDebug(bool a);
	~PanelDebug();

	void Draw();
	void LogDebug(const char* text);

private:
	ImGuiTextBuffer debug_buff;
	bool move_debug_scroll = false;
};

