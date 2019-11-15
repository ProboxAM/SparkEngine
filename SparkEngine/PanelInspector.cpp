#include "PanelInspector.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentCamera.h"
#include "ImGui/imgui_stdlib.h"

#include <iomanip>

PanelInspector::PanelInspector(bool a) : Panel(a)
{
}

PanelInspector::~PanelInspector()
{
}

void PanelInspector::Draw()
{
	ImGui::Begin("Inspector", &active);
	if (App->scene->selected_gameobject)
	{
		bool active = App->scene->selected_gameobject->isActive();
		if (ImGui::Checkbox(" ", &active))
		{
			App->scene->selected_gameobject->SetActive(active);
		}
		ImGui::SameLine();
		ImGui::PushID("name");
		std::string obj_name = App->scene->selected_gameobject->GetName();
		if (ImGui::InputText("", &obj_name, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			App->scene->selected_gameobject->SetName(obj_name);
		ImGui::PopID();

		ImGui::Separator();

		std::vector<Component*> comp = App->scene->selected_gameobject->GetComponents();
		GameObject* go = App->scene->selected_gameobject;
		for (int i = 0; i < comp.size(); i++)
		{
			if (comp[i]->type == COMPONENT_TYPE::TRANSFORM)
			{
				if (ImGui::CollapsingHeader("Transform"))
				{
					float* p = (float*)&go->transform->local_position;
					if (ImGui::InputFloat3("Position", p, 2)) {
						go->transform->UpdateTransformMatrix();
					}

					float* r = (float*)&go->transform->local_euler_rotation;
					if(ImGui::InputFloat3("Rotation", r, 2)) {
						go->transform->UpdateTransformMatrix();
					}

					float* s = (float*)&go->transform->local_scale;
					if (ImGui::InputFloat3("Scale", s, 2)) {
						go->transform->UpdateTransformMatrix();
					}
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::MESH)
			{
				ComponentMesh* c_mesh = (ComponentMesh*)go->GetComponent(COMPONENT_TYPE::MESH);

				if (ImGui::CollapsingHeader("Mesh")) 
				{
					ImGui::PushID("active_mesh");
					ImGui::Checkbox("Active", &c_mesh->active);
					ImGui::PopID();

					ImGui::TextColored({ 255, 255, 0, 255 }, ("Index amount: " + std::to_string(c_mesh->GetIndicesAmount()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Vertex amount: " + std::to_string(c_mesh->GetVerticesAmount())).c_str());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Normal amount: " + std::to_string(c_mesh->GetNormalsAmount()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("UV amount: " + std::to_string(c_mesh->GetUVAmount())).c_str());

					ImGui::Checkbox("Vertex normals: ", &c_mesh->debug_vertex_normal);
					ImGui::Checkbox("Face normals: ", &c_mesh->debug_face_normal);
					ImGui::Checkbox("Bounding box: ", &c_mesh->debug_bounding_box);
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::TEXTURE)
			{
				ComponentTexture* c_texture = (ComponentTexture*)go->GetComponent(COMPONENT_TYPE::TEXTURE);
				if (ImGui::CollapsingHeader("Texture"))
				{
					ImGui::PushID("active_texture");
					ImGui::Checkbox("Active", &c_texture->active);
					ImGui::PopID();

					ImGui::TextColored({ 255, 255, 0, 255 }, ("With: " + std::to_string(c_texture->GetTextureWidth()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Height: " + std::to_string(c_texture->GetTextureHeight())).c_str());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("File: " + c_texture->GetTexturePath()).c_str());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Size: " + std::to_string(c_texture->GetTextureSize()) + "b |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Depth: " + std::to_string(c_texture->GetTextureDepth())).c_str());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Format: " + c_texture->GetTextureFormat() + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("MIP: " + std::to_string(c_texture->GetTextureMIP()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("BPP: " + std::to_string(c_texture->GetTextureBPP())).c_str());

					ImGui::Image((void*)(intptr_t)c_texture->GetTextureID(), ImVec2(256, 256), ImVec2(0,1), ImVec2(1,0));


					ImGui::Checkbox("Checkers texture", &c_texture->debug_texture);
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::LIGHT)
			{
				ImGui::CollapsingHeader("LIGHT");
			}

			if (comp[i]->type == COMPONENT_TYPE::CAMERA)
			{
				ComponentCamera* c_camera = (ComponentCamera*)go->GetComponent(COMPONENT_TYPE::CAMERA);
				if (ImGui::CollapsingHeader("Camera")) {
					ImGui::PushID("active_texture");
					ImGui::Checkbox("Active", &c_camera->active);
					ImGui::PopID();

					float npd = c_camera->GetFrustumNearPlaneDistance();
					if (ImGui::SliderFloat("Near plane distance: ", &npd, 0.1f, 1.0f, "%.2f")) {
						c_camera->SetFrustumNearPlaneDistance(npd);
					}

					float fpd = c_camera->GetFrustumFarPlaneDistance();
					if (ImGui::SliderFloat("Far plane distance: ", &fpd, 1.f, 1000.f, "%.2f")) {
						c_camera->SetFrustumFarPlaneDistance(fpd);
					}

					float fov = c_camera->GetFrustumVerticalFOV();
					if (ImGui::SliderAngle("FOV: ", &fov, 60.0f, 90.0f)){
						c_camera->SetFrustumFOV(fov);
					}
				}
			}

			ImGui::Separator();
		}

		if (ImGui::Button("Add Component")) ImGui::OpenPopup("Add Component...");

		if (ImGui::BeginPopup("Add Component..."))
		{
			if (ImGui::Selectable("Camera")) {
				App->scene->selected_gameobject->AddComponent(COMPONENT_TYPE::CAMERA);
			}

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}
