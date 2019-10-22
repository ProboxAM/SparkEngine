#ifndef _PANEL_CONSOLE_H_
#define _PANEL_CONSOLE_H_

#include "Panel.h"
class PanelConsole : public Panel
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

#endif // !_PANEL_CONSOLE_H_



