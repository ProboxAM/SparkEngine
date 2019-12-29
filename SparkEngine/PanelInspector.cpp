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
#include "ResourceMesh.h"
#include "PanelAnimator.h"

#include "ModelMetaFile.h"
#include "AnimationMetaFile.h"
#include "TextureMetaFile.h"

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
	if (App->scene->selected_gameobject && App->scene->selected_gameobject != App->scene->root)
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
		ImGui::SameLine();
		bool is_static = App->scene->selected_gameobject->isStatic();
		if(ImGui::Checkbox("Static:", &is_static))
		{
			App->scene->SetGameObjectStatic(App->scene->selected_gameobject, is_static);
		}

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
						if (!go->isStatic()) {
							if (!App->scene->global_mode)go->transform->SetLocalPosition((float3)p);
							else go->transform->SetPosition((float3)p);
						}
					}

					float* r;
					if (!App->scene->global_mode)r = (float*)&go->transform->GetLocalRotationToEuler();
					else r = (float*)&go->transform->GetRotationToEuler();
					if(ImGui::InputFloat3("Rotation", r, 2)) {
						if (!go->isStatic()) {
							if (!App->scene->global_mode)go->transform->SetLocalRotationFromEuler((float3)r);
							else go->transform->SetRotationFromEuler((float3)r);
						}
					}

					float* s;
					if (!App->scene->global_mode)s = (float*)&go->transform->GetLocalScale();
					else s = (float*)&go->transform->GetScale();
					if (ImGui::InputFloat3("Scale", s, 2)) {
						if (!go->isStatic()) {
							if (!App->scene->global_mode)go->transform->SetLocalScale((float3)s);
							else go->transform->SetScale((float3)s);
						}
					}
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::MESH)
			{
				ComponentMesh* c_mesh = (ComponentMesh*)go->GetComponent(COMPONENT_TYPE::MESH);

				if (ImGui::CollapsingHeader("Mesh") && c_mesh->GetMesh()) 
				{
					ImGui::PushID("active_mesh");
					ImGui::Checkbox("Active", &c_mesh->active);
					ImGui::PopID();

					ImGui::Text("File: %s", c_mesh->GetMesh()->GetFile());
					ImGui::Text("ID: %u", c_mesh->GetMesh()->GetID());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Index amount: " + std::to_string(c_mesh->GetIndicesAmount()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Vertex amount: " + std::to_string(c_mesh->GetVerticesAmount())).c_str());
					ImGui::TextColored({ 255, 255, 0, 255 }, ("Normal amount: " + std::to_string(c_mesh->GetNormalsAmount()) + " |").c_str());
					ImGui::SameLine();
					ImGui::TextColored({ 255, 255, 0, 255 }, ("UV amount: " + std::to_string(c_mesh->GetUVAmount())).c_str());

					ImGui::Checkbox("Bounding box", &c_mesh->debug_bounding_box);

					if (c_mesh->root_bone_id != 0)
					{
						ImGui::Text("Root Bone: %s", App->scene->gameobjects[c_mesh->root_bone_id]->GetName().c_str());
						bool debug_skeleton = c_mesh->debug_skeleton;
						if (ImGui::Checkbox("Debug Skeleton", &debug_skeleton))
							c_mesh->SetDebugSkeleton(debug_skeleton);
					}
				}
			}
			if (comp[i]->type == COMPONENT_TYPE::TEXTURE)
			{
				ComponentTexture* c_texture = (ComponentTexture*)go->GetComponent(COMPONENT_TYPE::TEXTURE);
				if (ImGui::CollapsingHeader("Texture") && c_texture->GetTexture())
				{
					ImGui::PushID("active_texture");
					ImGui::Checkbox("Active", &c_texture->active);
					ImGui::PopID();

					ImGui::Text("File: %s", c_texture->GetTexture()->GetFile());
					ImGui::Text("ID: %u", c_texture->GetTexture()->GetID());
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

			if (comp[i]->type == COMPONENT_TYPE::ANIMATOR)
			{
				ComponentAnimator* c_anim = (ComponentAnimator*)go->GetComponent(COMPONENT_TYPE::ANIMATOR);
				if (ImGui::CollapsingHeader("Animator")) {
					if (ImGui::Selectable("Animator Controller")) {				
						ImGui::OpenPopup("Select Animator Controller");
						selected_animator = c_anim;
					}
					if (c_anim->GetResourceAnimatorController())
						ImGui::Text(c_anim->GetResourceAnimatorController()->name.c_str());
					else
						ImGui::Text("No Controller");
				}
			}

			if (comp[i]->type == COMPONENT_TYPE::CAMERA)
			{
				ComponentCamera* c_camera = (ComponentCamera*)go->GetComponent(COMPONENT_TYPE::CAMERA);
				if (ImGui::CollapsingHeader("Camera")) {
					ImGui::PushID("active_texture");
					ImGui::Checkbox("Active", &c_camera->active);
					ImGui::PopID();

					ImGui::SameLine();
					bool main_camera = c_camera->active_camera;
					if (ImGui::Checkbox("Main Camera", &main_camera)) {
						c_camera->SetAsMainCamera(main_camera);
					}

					ImGui::SameLine();
					ImGui::Checkbox("Enable Frustum Culling", &c_camera->enable_frustum_culling);

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

			if (ImGui::Selectable("Animator")) {
				ComponentAnimator* ca = (ComponentAnimator*)App->scene->selected_gameobject->AddComponent(COMPONENT_TYPE::ANIMATOR);
			}

			ImGui::EndPopup();
		}
	}
	else if (((PanelProject*)App->editor->GetPanel(Panel_Type::PROJECT))->selected_item != "")
	{
		PanelProject* project_panel = (PanelProject*)App->editor->GetPanel(Panel_Type::PROJECT);
		std::string file = project_panel->selected_item;
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
		else
			ImGui::Separator();
	}

	if (ImGui::BeginPopup("Select Animator Controller"))
	{
		ImGui::Text("Animator Controllers");
		ImGui::Separator();

		std::vector<Resource*> animator_controllers = App->resources->GetResources(Resource::RESOURCE_TYPE::R_ANIMATOR);

		for (int i = 0; i < animator_controllers.size(); i++) {
			if (ImGui::Selectable(((ResourceAnimatorController*)animator_controllers[i])->name.c_str())) {
				selected_animator->SetAnimatorController((ResourceAnimatorController*)animator_controllers[i]);

				PanelAnimator* pa = (PanelAnimator*)App->editor->GetPanel(Panel_Type::P_ANIMATOR);
				pa->SetCurrentResourceAnimatorController((ResourceAnimatorController*)animator_controllers[i]);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndPopup();
	}
	ImGui::End();
}

void PanelInspector::ShowTextureImportSettings(Resource* res)
{
	TextureMetaFile* meta = (TextureMetaFile*) res->meta;

	
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
				meta->compression = (TextureMetaFile::TEXTURE_COMPRESSION) i;
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
				meta->wrap_s = (TextureMetaFile::TEXTURE_WRAP_MODE) i;
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
				meta->wrap_t = (TextureMetaFile::TEXTURE_WRAP_MODE) i;
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
				meta->min_filter = (TextureMetaFile::TEXTURE_FILTER_MODE) i;
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
				meta->mag_filter = (TextureMetaFile::TEXTURE_FILTER_MODE) i;
		ImGui::EndPopup();
	}
	ImGui::PopID();

	ImGui::Separator();
	if (ImGui::Button("Save")) {
		App->importer->texture->SaveMeta(meta);
		LOG("Reimporting texture with new settings...");
		App->resources->ImportFile(meta->original_file.c_str(), Resource::RESOURCE_TYPE::R_TEXTURE, meta);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		meta->SetDefault();
	}

	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("image", { ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2 }, false, ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::Image((void*)(intptr_t)((ResourceTexture*)res)->buffer_id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
}

void PanelInspector::ShowModelImportSettings(Resource* res)
{
	ModelMetaFile* meta = (ModelMetaFile*) res->meta;

	ImGui::PushID("Model_Button");
	if (selected_model_import_section == ModelImportSettingSection::I_MODEL)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		if (ImGui::Button("Model"))
			selected_model_import_section = ModelImportSettingSection::I_MODEL;
		ImGui::PopStyleVar();
	}
	else
	{
		if (ImGui::Button("Model"))
			selected_model_import_section = ModelImportSettingSection::I_MODEL;
	}
	ImGui::PopID();

	ImGui::SameLine();

	ImGui::PushID("Animation_Button");
	if (selected_model_import_section == ModelImportSettingSection::I_ANIMATION)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		if (ImGui::Button("Animation"))
			selected_model_import_section = ModelImportSettingSection::I_ANIMATION;
		ImGui::PopStyleVar();
	}
	else
	{
		if (ImGui::Button("Animation"))
			selected_model_import_section = ModelImportSettingSection::I_ANIMATION;
	}
	ImGui::PopID();

	if (selected_model_import_section == ModelImportSettingSection::I_MODEL)
	{
		const char* presets[] = { "Max Quality", "Quality", "Fast", "Custom" };

		ImGui::Text("Import Preset: ");
		ImGui::SameLine();
		if (ImGui::Button(presets[meta->GetSelectedPreset()]))
			ImGui::OpenPopup("Import_preset");
		if (ImGui::BeginPopup("Import_preset"))
		{
			for (int i = 0; i < 4; i++)
				if (ImGui::Selectable(presets[i]))
					meta->SetImportSettings((ModelMetaFile::MODEL_IMPORT_SETTING)i);
			ImGui::EndPopup();
		}

		bool disabled = meta->GetSelectedPreset() != ModelMetaFile::CUSTOM;
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
		if (ImGui::Button("Apply")) {
			App->importer->model->SaveMeta(meta);
			LOG("Reimporting model with new settings...");
			App->resources->ImportFile(meta->original_file.c_str(), Resource::RESOURCE_TYPE::R_MODEL, meta);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset")) {
			meta->SetDefault();
		}
	}
	else
	{
		ImGui::Checkbox("Import Animation", &meta->import_animation);
		ImGui::Separator();
		static std::string clip_name;
		for each (AnimationMetaFile* anim_meta in meta->animations)
		{
			ImGui::PushID(anim_meta);
			clip_name = anim_meta->name;
			if (ImGui::InputText("Clip Name", &clip_name, ImGuiInputTextFlags_AutoSelectAll))
				anim_meta->name = clip_name;
			int start_tick = (int)anim_meta->start_tick;
			int end_tick = (int)anim_meta->end_tick;
			if (ImGui::DragInt("Start", &start_tick, 1.0F, 0, anim_meta->end_tick - 1))
				if(start_tick >= 0 && start_tick < anim_meta->end_tick) anim_meta->start_tick = (uint)start_tick;
			if (ImGui::DragInt("End", &end_tick, 1.0F, anim_meta->start_tick + 1, meta->max_ticks))
				if (end_tick > anim_meta->start_tick && end_tick <= meta->max_ticks) anim_meta->end_tick = (uint)end_tick;
			ImGui::Checkbox("Loops", &anim_meta->loops);
			ImGui::Separator();
			ImGui::PopID();
		}
		if (ImGui::Button("+"))
		{
			AnimationMetaFile* new_anim = new AnimationMetaFile();
			new_anim->name = "New Clip";
			new_anim->max_tick = meta->max_ticks;
			new_anim->end_tick = meta->max_ticks;
			new_anim->id = App->GenerateID();
			meta->animations.push_back(new_anim);
		}
		ImGui::SameLine();
		if(ImGui::Button("-") && meta->animations.size() > 0)
		{
			delete meta->animations[meta->animations.size()-1];
			meta->animations.pop_back();
		}

		ImGui::Separator();
		if (ImGui::Button("Apply")) {
			App->importer->model->SaveMeta(meta);
			LOG("Reimporting model with new settings...");
			App->resources->ImportFile(meta->original_file.c_str(), Resource::RESOURCE_TYPE::R_MODEL, meta);

			for each (AnimationMetaFile* anim_meta in meta->animations)
			{
				ResourceAnimation* r_anim = (ResourceAnimation*) App->resources->Get(anim_meta->id);
				if (!r_anim->IsLoaded())
					r_anim->AddReference();
			}
		}
	}
}