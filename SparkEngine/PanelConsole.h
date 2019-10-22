#pragma once
#include "Panel.h"
class PanelConsole :
	public Panel
{
public:
	PanelConsole(bool a);
	~PanelConsole();

	void Draw();
	void LogDebug(const char* text);

private:
	ImGuiTextBuffer debug_buff;
	bool move_debug_scroll = false;
};

