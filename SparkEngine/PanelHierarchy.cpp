#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"

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
	for (int i = 0; i < App->scene->gameobjects.size(); ++i)
	{
		if (App->scene->gameobjects[i]->GetName() != "root")
		{
			if (ImGui::Selectable(App->scene->gameobjects[i]->GetName().c_str(), selected == i))
			{
				selected = i;
				App->scene->selected_gameobject = App->scene->gameobjects[i];
				//LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
			}
		}
	}

	ImGui::End();
}
