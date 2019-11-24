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

	const char* modes[] = { "All", "Loaded", "Not Loaded"};

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
		if (mode == Mode::LOADED && !it->second->IsLoaded())
			continue;
		else if (mode == Mode::NOT_LOADED && it->second->IsLoaded())
			continue;

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
