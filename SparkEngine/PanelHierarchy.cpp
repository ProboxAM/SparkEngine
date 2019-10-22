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
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	static int selected = -1;
	for (int i = 0; i < App->scene->root->transform->GetChildCount(); ++i)
	{
		ImGuiTreeNodeFlags node_flags = base_flags;
		if (App->scene->root->transform->GetChildren()[i]->GetChildCount() > 0)
		{
			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, App->scene->root->transform->GetChildren()[i]->gameobject->GetName().c_str(), i);
			if (ImGui::IsItemClicked()) {
				selected = i;
				App->scene->selected_gameobject = App->scene->root->transform->GetChildren()[i]->gameobject;
				LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
			}
			if (node_open) {
				for (int j = 0; j < App->scene->root->transform->GetChildren()[i]->GetChildCount(); j++)
				{
					node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
					ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, App->scene->root->transform->GetChildren()[i]->GetChildren()[j]->gameobject->GetName().c_str(), i);
					if (ImGui::IsItemClicked()) {
						selected = j;
						App->scene->selected_gameobject = App->scene->root->transform->GetChildren()[i]->GetChildren()[j]->gameobject;
						LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
					}
				}
				ImGui::TreePop();
			}

		}
		else {
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
			ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, App->scene->root->transform->GetChildren()[i]->gameobject->GetName().c_str(), i);
			if (ImGui::IsItemClicked()) {
				selected = i;
				App->scene->selected_gameobject = App->scene->root->transform->GetChildren()[i]->gameobject;
				LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
			}
		}
	}

	ImGui::End();
}
