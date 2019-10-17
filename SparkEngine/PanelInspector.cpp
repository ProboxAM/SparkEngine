#include "PanelInspector.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"

#include <iomanip>

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
					std::stringstream stream;
					stream << std::fixed << std::setprecision(2) << go->transform->position.x;
					x = stream.str();
					stream.str("");
					stream << std::fixed << std::setprecision(2) << go->transform->position.y;
					y = stream.str();
					stream.str("");
					stream << std::fixed << std::setprecision(2) << go->transform->position.z;
					z = stream.str();
					stream.str("");
					ImGui::TextColored({ 255, 255, 0, 255 }, ("X: " + x + " Y: " + y + " Z: " + z).c_str());

					ImGui::Text("Rotation: ");
					ImGui::SameLine();
					stream << std::fixed << std::setprecision(2) << go->transform->EulerAngles().x;
					x = stream.str();
					stream.str("");
					stream << std::fixed << std::setprecision(2) << go->transform->EulerAngles().y;
					y = stream.str();
					stream.str("");
					stream << std::fixed << std::setprecision(2) << go->transform->EulerAngles().z;
					z = stream.str();
					stream.str("");
					ImGui::TextColored({ 255, 255, 0, 255 }, ("X: " + x + " Y: " + y + " Z: " + z).c_str());

					ImGui::Text("Scale: ");
					ImGui::SameLine();
					stream << std::fixed << std::setprecision(2) << go->transform->scale.x;
					x = stream.str();
					stream.str("");
					stream << std::fixed << std::setprecision(2) << go->transform->scale.y;
					y = stream.str();
					stream.str("");
					stream << std::fixed << std::setprecision(2) << go->transform->scale.z;
					z = stream.str();
					stream.str("");
					ImGui::TextColored({ 255, 255, 0, 255 }, ("   X: " + x + " Y: " + y + " Z: " + z).c_str());
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::MESH)
			{
				ComponentMesh* c_mesh = (ComponentMesh*)go->GetComponent(COMPONENT_TYPE::MESH);

				if (ImGui::CollapsingHeader("Mesh")) 
				{
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Index amount: " + std::to_string(c_mesh->GetIndicesAmount()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Vertex amount: " + std::to_string(c_mesh->GetVerticesAmount())).c_str());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Normal amount: " + std::to_string(c_mesh->GetNormalsAmount()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("UV amount: " + std::to_string(c_mesh->GetUVAmount())).c_str());

					ImGui::Checkbox("Vertex normals: ", &c_mesh->debug_vertex_normal);
					ImGui::Checkbox("Face normals: ", &c_mesh->debug_face_normal);
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::TEXTURE)
			{
				ComponentTexture* c_texture = (ComponentTexture*)go->GetComponent(COMPONENT_TYPE::TEXTURE);
				if (ImGui::CollapsingHeader("Texture"))
				{
					ImGui::TextColored({ 255, 255, 0, 255 }, ("With: " + std::to_string(c_texture->GetTextureWidth()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Height: " + std::to_string(c_texture->GetTextureHeight())).c_str());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Path: " + c_texture->GetTexturePath()).c_str());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Size: " + std::to_string(c_texture->GetTextureSize()) + "b |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Depth: " + std::to_string(c_texture->GetTextureDepth())).c_str());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Format: " + c_texture->GetTextureFormat() + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("MIP: " + std::to_string(c_texture->GetTextureMIP()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("BPP: " + std::to_string(c_texture->GetTextureBPP())).c_str());

					ImGui::Image((void*)(intptr_t)c_texture->GetTextureID(), ImVec2(256, 256));


					ImGui::Checkbox("Checkers texture", &c_texture->debug_texture);
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::LIGHT)
			{
				ImGui::CollapsingHeader("LIGHT");
			}
		}
	}
	ImGui::End();
}
