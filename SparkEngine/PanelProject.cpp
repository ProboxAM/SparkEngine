#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleImporter.h"
#include "ModuleResources.h"
#include "TextureImporter.h"
#include "ResourceTexture.h"

#include "PanelProject.h"


PanelProject::PanelProject(bool active): Panel(active)
{
	image_size = 50;
	text_size = 20;
}


PanelProject::~PanelProject()
{
}

void PanelProject::Draw()
{
	ImGui::Begin("Project", &active);

	DrawFiles();

	ImGui::End();
}

void PanelProject::DrawFiles()
{
	ImGui::Text("Assets");
	ImGui::Separator();

	ImVec2 pos = ImGui::GetCursorPos();

	std::vector<std::string> files;
	App->fsystem->GetFilesFiltered(ASSETS_FOLDER, files, "meta");

	for (int i = 0; i < files.size(); ++i)
	{
		ImGui::SameLine();
		ImGui::BeginChild(i + 1, { (float)image_size, (float)image_size + text_size }, false, ImGuiWindowFlags_NoScrollbar);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			uint id = App->resources->GetID(ASSETS_FOLDER + files[i]);
			ImGui::SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &id, sizeof(uint));
			ImGui::EndDragDropSource();
		}

		ImGui::Image((void*)(intptr_t)((ResourceTexture*)App->resources->Get(App->importer->texture->checkers))->buffer_id,
			ImVec2(image_size, image_size), ImVec2(0, 1), ImVec2(1, 0));

		if (ImGui::IsItemClicked())
		{
			selected_file = files[i];

			uint id = App->resources->GetID(ASSETS_FOLDER + files[i]);
			selected_resource = App->resources->Get(id);

			App->scene->selected_gameobject = nullptr;
		}

		ImGui::Text(files[i].c_str());

		ImGui::EndChild();
	}
}