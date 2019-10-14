#include "PanelInspector.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"


PanelInspector::PanelInspector(bool a) : Panel(a)
{
}

PanelInspector::~PanelInspector()
{
}

void PanelInspector::Draw()
{
	ImGui::Begin("Inspector");
	if (App->scene->selected_gameobject)
	{
		std::vector<Component*> comp = App->scene->selected_gameobject->GetComponents(COMPONENT_TYPE::NONE);
		GameObject* go = App->scene->selected_gameobject;
		for (int i = 0; i < comp.size(); i++)
		{
			if (comp[i]->type == COMPONENT_TYPE::TRANSFORM)
			{
				if (ImGui::CollapsingHeader("Transform"))
				{
					ImGui::Text("Position: ");
					ImGui::SameLine();
					std::string x, y, z;
					x = std::to_string(go->transform->position.x);
					y = std::to_string(go->transform->position.y);
					z = std::to_string(go->transform->position.z);
					ImGui::TextColored({ 255, 255, 0, 255 }, ("X: " + x + " Y: " + y + " Z: " + z).c_str());

					ImGui::Text("Rotation: ");
					ImGui::SameLine();
					x = std::to_string(go->transform->EulerAngles().x);
					y = std::to_string(go->transform->EulerAngles().y);
					z = std::to_string(go->transform->EulerAngles().z);
					ImGui::TextColored({ 255, 255, 0, 255 }, ("X: " + x + " Y: " + y + " Z: " + z).c_str());

					ImGui::Text("Scale: ");
					ImGui::SameLine();
					x = std::to_string(go->transform->scale.x);
					y = std::to_string(go->transform->scale.y);
					z = std::to_string(go->transform->scale.z);
					ImGui::TextColored({ 255, 255, 0, 255 }, ("   X: " + x + " Y: " + y + " Z: " + z).c_str());
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::MESH)
			{
				ComponentMesh* c_mesh = (ComponentMesh*)go->GetComponent(COMPONENT_TYPE::MESH);

				if (ImGui::CollapsingHeader("Mesh")) 
				{
					
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::TEXTURE)
			{
				ImGui::CollapsingHeader("Texture");
			}
			if (comp[i]->type == COMPONENT_TYPE::LIGHT)
			{
				ImGui::CollapsingHeader("LIGHT");
			}
		}
	}
	ImGui::End();
}
