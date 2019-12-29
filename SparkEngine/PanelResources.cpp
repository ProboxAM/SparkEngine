#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"
#include "PanelResources.h"



PanelResources::PanelResources(bool active) : Panel(active)
{
}


PanelResources::~PanelResources()
{
}

void PanelResources::Draw()
{
	ImGui::Begin("Resources", &active);

	const char* modes[] = { "All", "Loaded", "Not Loaded", "Mesh","Texture","Animation","Anim Controller","Bone"};

	ImGui::Text("Show:");
	ImGui::SameLine();
	if (ImGui::Button(modes[mode]))
		ImGui::OpenPopup("Filter_Selection");
	if (ImGui::BeginPopup("Filter_Selection"))
	{
		for (int i = 0; i < Mode::TOTAL; i++)
			if (ImGui::Selectable(modes[i]))
				mode = (Mode)i;
		ImGui::EndPopup();
	}

	std::map<uint, Resource*> resources = App->resources->GetResources();
	for(std::map<uint, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		switch (mode)
		{
		case PanelResources::ALL:
			break;
		case PanelResources::LOADED:
			if (!it->second->IsLoaded())
				continue;
			break;
		case PanelResources::NOT_LOADED:
			if (it->second->IsLoaded())
				continue;
			break;
		case PanelResources::MESH:
			if (it->second->GetType() != Resource::RESOURCE_TYPE::R_MESH)
				continue;
			break;
		case PanelResources::TEXTURE:
			if (it->second->GetType() != Resource::RESOURCE_TYPE::R_TEXTURE)
				continue;
			break;
		case PanelResources::ANIMATION:
			if (it->second->GetType() != Resource::RESOURCE_TYPE::R_ANIMATION)
				continue;
			break;
		case PanelResources::BONE:
			if (it->second->GetType() != Resource::RESOURCE_TYPE::R_BONE)
				continue;
			break;
		case PanelResources::ANIMATOR:
			if (it->second->GetType() != Resource::RESOURCE_TYPE::R_ANIMATOR)
				continue;
			break;
		case PanelResources::TOTAL:
			break;
		default:
			break;
		}

		ImGui::Separator();
		ImGui::Text("Original file: %s", it->second->GetFile());
		ImGui::Text("Library file: %s", it->second->GetExportedFile());
		ImGui::Text("Type: %s", it->second->GetTypeString().c_str());
		ImGui::Text("ID: %u", it->first);
		ImGui::Text("References: %i", it->second->GetReferences());

		ImGui::SameLine();
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		bool loaded = it->second->IsLoaded();
		ImGui::Checkbox("Loaded", &loaded);
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::End();
}
