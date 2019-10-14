#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

PanelHierarchy::PanelHierarchy(bool a) : Panel(a)
{

}

PanelHierarchy::~PanelHierarchy()
{
}

void PanelHierarchy::Draw()
{
	ImGui::Begin("Hierarchy");
	static int selected = -1;
	for (int i = 0; i < App->scene->gameobjects.size(); i++)
	{
		if (ImGui::Selectable(App->scene->gameobjects[i]->GetName().c_str()), selected == i)
		{
			selected = i;
			App->scene->selected_gameobject = App->scene->gameobjects[i];
		}
	}

	ImGui::End();
}
