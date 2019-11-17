#include "PanelInspector.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ModuleImporter.h"
#include "TextureImporter.h"
#include "ModelImporter.h"
#include "GameObject.h"
#include "PanelProject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentCamera.h"
#include "ResourceModel.h"
#include "ResourceTexture.h"

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
					float* p;			
					if(!App->scene->global_mode) p = (float*)&go->transform->GetLocalPosition();
					else p = (float*)&go->transform->GetPosition();
					if (ImGui::InputFloat3("Position", p, 2)) {
						if (!App->scene->global_mode)go->transform->SetLocalPosition((float3)p);
						else go->transform->SetPosition((float3)p);
					}

					float* r;
					if (!App->scene->global_mode)r = (float*)&go->transform->GetLocalRotationToEuler();
					else r = (float*)&go->transform->GetRotationToEuler();
					if(ImGui::InputFloat3("Rotation", r, 2)) {
						if (!App->scene->global_mode)go->transform->SetLocalRotationFromEuler((float3)r);
						else go->transform->SetRotationFromEuler((float3)r);
					}

					float* s;
					if (!App->scene->global_mode)s = (float*)&go->transform->GetLocalScale();
					else s = (float*)&go->transform->GetScale();
					if (ImGui::InputFloat3("Scale", s, 2)) {
						if (!App->scene->global_mode)go->transform->SetLocalScale((float3)s);
						else go->transform->SetScale((float3)s);
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
	else if (((PanelProject*)App->editor->GetPanel(Panel_Type::PROJECT))->selected_file != "")
	{
		PanelProject* project_panel = (PanelProject*)App->editor->GetPanel(Panel_Type::PROJECT);
		std::string file = project_panel->selected_file;
		ImGui::Text(file.c_str());
		if (project_panel->selected_resource)
		{
			ImGui::SameLine();
			ImGui::Text("Import Settings");
			ImGui::Separator();

			switch (project_panel->selected_resource->GetType())
			{
				case Resource::RESOURCE_TYPE::R_TEXTURE:
					ShowTextureImportSettings(project_panel->selected_resource);
					break;
				case Resource::RESOURCE_TYPE::R_MODEL:
					ShowModelImportSettings(project_panel->selected_resource);
					break;
			}
		}
	}
	ImGui::End();
}

void PanelInspector::ShowTextureImportSettings(Resource* res)
{
	ResourceTexture::TextureMetaFile* meta = (ResourceTexture::TextureMetaFile*) res->meta;

	
	ImGui::BeginChild("settings", { ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2 }, false, ImGuiWindowFlags_NoScrollWithMouse);
	const char* compressions[] = { "DXT1", "DXT2", "DXT5"};

	ImGui::Text("Compression: ");
	ImGui::SameLine();
	if (ImGui::Button(compressions[meta->compression]))
		ImGui::OpenPopup("Compression_popup");
	if (ImGui::BeginPopup("Compression_popup"))
	{
		for (int i = 0; i < 3; i++)
			if (ImGui::Selectable(compressions[i]))
				meta->compression = (ResourceTexture::TextureMetaFile::TEXTURE_COMPRESSION) i;
		ImGui::EndPopup();
	}

	const char* wrap_modes[] = { "Repeat", "Mirrored Repeat", "Clamp", "Clamp to Border" };
	
	ImGui::PushID("wrap_s");
	ImGui::Text("Wrap Mode S: ");
	ImGui::SameLine();
	if (ImGui::Button(wrap_modes[meta->wrap_s]))
		ImGui::OpenPopup("wrap_mode_s");
	if (ImGui::BeginPopup("wrap_mode_s"))
	{
		for (int i = 0; i < 4; i++)
			if (ImGui::Selectable(wrap_modes[i]))
				meta->wrap_s = (ResourceTexture::TextureMetaFile::TEXTURE_WRAP_MODE) i;
		ImGui::EndPopup();
	}
	ImGui::PopID();

	ImGui::PushID("wrap_t");
	ImGui::Text("Wrap Mode T: ");
	ImGui::SameLine();
	if (ImGui::Button(wrap_modes[meta->wrap_t]))
		ImGui::OpenPopup("wrap_mode_t");
	if (ImGui::BeginPopup("wrap_mode_t"))
	{
		for (int i = 0; i < 4; i++)
			if (ImGui::Selectable(wrap_modes[i]))
				meta->wrap_t = (ResourceTexture::TextureMetaFile::TEXTURE_WRAP_MODE) i;
		ImGui::EndPopup();
	}
	ImGui::PopID();

	const char* filter_modes[] = { "Nearest", "Linear"};

	ImGui::PushID("filter_min");
	ImGui::Text("Min Filter:  ");
	ImGui::SameLine();
	if (ImGui::Button(filter_modes[meta->min_filter]))
		ImGui::OpenPopup("filter_min");
	if (ImGui::BeginPopup("filter_min"))
	{
		for (int i = 0; i < 2; i++)
			if (ImGui::Selectable(filter_modes[i]))
				meta->min_filter = (ResourceTexture::TextureMetaFile::TEXTURE_FILTER_MODE) i;
		ImGui::EndPopup();
	}
	ImGui::PopID();

	ImGui::PushID("filter_mag");
	ImGui::Text("Mag Filter:  ");
	ImGui::SameLine();
	if (ImGui::Button(filter_modes[meta->mag_filter]))
		ImGui::OpenPopup("mag_filter");
	if (ImGui::BeginPopup("mag_filter"))
	{
		for (int i = 0; i < 2; i++)
			if (ImGui::Selectable(filter_modes[i]))
				meta->mag_filter = (ResourceTexture::TextureMetaFile::TEXTURE_FILTER_MODE) i;
		ImGui::EndPopup();
	}
	ImGui::PopID();

	ImGui::Separator();
	if (ImGui::Button("Save")) {
		App->importer->texture->SaveMeta(meta);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		meta->SetDefault();
	}

	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("image", { ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2 }, false, ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::Image((void*)(intptr_t)((ResourceTexture*)App->resources->Get(App->importer->texture->checkers))->buffer_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
}

void PanelInspector::ShowModelImportSettings(Resource* res)
{
	ResourceModel::ModelMetaFile* meta = (ResourceModel::ModelMetaFile*) res->meta;

	const char* presets[] = { "Max Quality", "Quality", "Fast", "Custom" };

	ImGui::Text("Import Preset: ");
	ImGui::SameLine();
	if (ImGui::Button(presets[meta->GetSelectedPreset()]))
		ImGui::OpenPopup("Import_preset");
	if (ImGui::BeginPopup("Import_preset"))
	{
		for (int i = 0; i < 4; i++)
			if (ImGui::Selectable(presets[i]))
				meta->SetImportSettings((ResourceModel::ModelMetaFile::MODEL_IMPORT_SETTING)i);
		ImGui::EndPopup();
	}

	bool disabled = meta->GetSelectedPreset() != ResourceModel::ModelMetaFile::CUSTOM;
	if (disabled)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::Checkbox("Generate Smooth Normals", &meta->gen_smooth_normals);
	ImGui::Checkbox("Optimize Meshes", &meta->optimize_meshes);
	ImGui::Checkbox("Join Vertices", &meta->join_vertices);
	ImGui::Checkbox("Validate Structures", &meta->validate_structures);
	ImGui::Checkbox("Split Large Meshes", &meta->split_large_meshes);
	ImGui::Checkbox("Generate UV Coords", &meta->gen_uv_coords);
	ImGui::Checkbox("Limit Bone Weights", &meta->limit_bone_weigths);
	ImGui::Checkbox("Remove Redundant Mats", &meta->remove_redundant_mats);
	ImGui::Checkbox("Triangulate", &meta->triangulate);
	ImGui::Checkbox("Sort by Type", &meta->sort_by_type);
	ImGui::Checkbox("Improve Cache Locality", &meta->improve_cache_locality);
	ImGui::Checkbox("Find Degenerates", &meta->find_degenerates);
	ImGui::Checkbox("Find Invalid Data", &meta->find_invalid_data);
	ImGui::Checkbox("Find Instances", &meta->find_instances);

	if (disabled)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::Separator();
	if (ImGui::Button("Save")) {
		App->importer->model->SaveMeta(meta);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		meta->SetDefault();
	}
}