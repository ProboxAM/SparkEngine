#include "PanelInspector.h"


PanelInspector::PanelInspector(bool a) : Panel(a)
{
}

PanelInspector::~PanelInspector()
{
}

void PanelInspector::Draw()
{
	ImGui::ShowDemoWindow();
}
