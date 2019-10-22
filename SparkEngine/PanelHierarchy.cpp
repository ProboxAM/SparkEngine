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
		if (App->scene->gameobjects[i] != App->scene->root)
		{
			if (App->scene->gameobjects[i]->transform->GetChildCount() > 0)
			{
				if (ImGui::TreeNode(App->scene->gameobjects[i]->GetName().c_str())) {
					for (int j = 0; j < App->scene->gameobjects[i]->transform->GetChildCount(); j++)
					{
						if (ImGui::Selectable(App->scene->gameobjects[i]->transform->GetChildren()[j]->gameobject->GetName().c_str(), selected == j))
						{
							selected = j;
							App->scene->selected_gameobject = App->scene->gameobjects[i]->transform->GetChildren()[j]->gameobject;
							LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
						}
					}

					ImGui::TreePop();
				}

			}
			else if(App->scene->gameobjects[i]->transform->GetParent()->gameobject == App->scene->root){
				if (ImGui::Selectable(App->scene->gameobjects[i]->GetName().c_str(), selected == i))
				{
					selected = i;
					App->scene->selected_gameobject = App->scene->gameobjects[i];
					LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
				}
			}
		}

	}

	ImGui::End();
}
