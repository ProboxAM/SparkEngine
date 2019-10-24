#include "PanelConsole.h"

PanelConsole::PanelConsole(bool a) : Panel(a)
{
}

PanelConsole::~PanelConsole()
{
}

void PanelConsole::Draw()
{
	ImGui::Begin("Console", &active);
	ImGui::TextUnformatted(debug_buff.begin());
	if (move_debug_scroll)
	{
		ImGui::SetScrollHere(1.0f);
		move_debug_scroll = false;
	}
	ImGui::End();
}

void PanelConsole::LogDebug(const char * text)
{
	debug_buff.appendf(text);
	move_debug_scroll = true;
}
