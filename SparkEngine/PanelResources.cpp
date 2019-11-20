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

	std::map<uint, Resource*> resources = App->resources->GetResources();
	for(std::map<uint, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
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
