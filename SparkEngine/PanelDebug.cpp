#include "PanelDebug.h"

PanelDebug::PanelDebug(bool a) : Panel(a)
{
}


PanelDebug::~PanelDebug()
{
}

void PanelDebug::Draw()
{
	ImGui::Begin("Console");
	ImGui::TextUnformatted(debug_buff.begin());
	if (move_debug_scroll)
	{
		ImGui::SetScrollHere(1.0f);
		move_debug_scroll = false;
	}
	ImGui::End();
}

void PanelDebug::LogDebug(const char * text)
{
	debug_buff.appendf(text);
	move_debug_scroll = true;
}
