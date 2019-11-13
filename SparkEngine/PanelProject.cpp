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
	columns = 10;
	image_size = 50;
	spacing = 30;
	offset = 10;
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

	int line = 0;

	for (int i = 0; i < files.size(); ++i)
	{
		ImGui::PushID(i);

		ImGui::SetCursorPosX(pos.x + (i - (line * columns)) * (image_size + spacing) + offset);
		ImGui::SetCursorPosY(pos.y + line * (image_size + spacing) + offset);

		ImGui::Image((void*)(intptr_t)((ResourceTexture*)App->resources->Get(App->importer->texture->checkers))->buffer_id,
			ImVec2(image_size, image_size), ImVec2(0, 1), ImVec2(1, 0));

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			uint id = (uint)App->resources->GetID(ASSETS_FOLDER + files[i]);
			ImGui::SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &id, sizeof(uint));
			ImGui::EndDragDropSource();
		}

		ImGui::SetCursorPosX(pos.x + (i - (line * columns)) * (image_size + spacing) + offset);
		ImGui::SetCursorPosY(pos.y + line * (image_size + spacing) + image_size + offset + offset);

		std::string text = FitTextToImage(files[i]);
		ImGui::Text(text.c_str());

		if ((i + 1) % columns == 0)
			line++;

		ImGui::PopID();
	}
}

std::string PanelProject::FitTextToImage(std::string text)
{
	std::string fitted_text = text;
	uint length = text.size();

	uint text_size = ImGui::CalcTextSize(text.c_str()).x;
	uint dots_size = ImGui::CalcTextSize("...").x;

	if (text_size > image_size)
	{
		uint char_size = text_size / length;
		uint new_length = ((image_size - dots_size) / char_size);
		fitted_text = fitted_text.substr(0, new_length);
		fitted_text.append("...");
	}

	return fitted_text;
}