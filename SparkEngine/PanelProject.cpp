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
	image_size = 60;
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
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::GetWindowWidth() * 0.1f);
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
	ImGui::SliderInt(" ", &image_size, 25, 100," ");
	ImGui::Separator();

	uint childs = 1;
	for (std::map<std::string,ResourceTexture*>::iterator it = assets_in_folder.begin(); it != assets_in_folder.end(); ++it)
	{
		ImGui::SameLine();
		ImGui::BeginChild(childs, { (float)image_size, (float)image_size + text_size }, false, ImGuiWindowFlags_NoScrollbar);

		uint id = App->resources->GetID(ASSETS_FOLDER + it->first);
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("ASSET", &id, sizeof(uint));
			ImGui::EndDragDropSource();
		}

		ImGui::Image((void*)(intptr_t)it->second->buffer_id,
			ImVec2(image_size * 0.9f, image_size), ImVec2(0, 1), ImVec2(1, 0));
		ManageClicksForItem(it->first);

		ImGui::Text(it->first.c_str());
		ManageClicksForItem(it->first);

		ImGui::EndChild();
		childs++;
	}
}

void PanelProject::Start()
{
	ResourceTexture* model_file_image = (ResourceTexture*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_TEXTURE, App->GenerateID());
	model_file_image->SetExportedFile(SETTINGS_FOLDER + std::string("fbx_file_image.png"));
	model_file_image->meta = new ResourceTexture::TextureMetaFile();

	ResourceTexture* scene_file_image = (ResourceTexture*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_TEXTURE, App->GenerateID());
	scene_file_image->SetExportedFile(SETTINGS_FOLDER + std::string("scene_file_image.png"));
	scene_file_image->meta = new ResourceTexture::TextureMetaFile();

	std::vector<std::string> files;
	App->fsystem->GetFilesFiltered(ASSETS_FOLDER, files, "meta");

	//Add reference to textures inside selected folder as they are being used here. If folder changes unreference texture
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		ResourceTexture* tex = nullptr;
		std::string extension;
		App->fsystem->SplitFilePath((*it).c_str(), nullptr, nullptr, &extension);
		Resource::RESOURCE_TYPE type = App->resources->GetTypeFromExtension(extension);

		switch (type)
		{
		case Resource::RESOURCE_TYPE::R_TEXTURE:
		{
			uint id = App->resources->GetID(ASSETS_FOLDER + (*it));
			tex = (ResourceTexture*)App->resources->GetAndReference(id);
		}
			break;
		case Resource::RESOURCE_TYPE::R_MODEL:
		{
			model_file_image->AddReference();
			tex = model_file_image;
		}
			break;
		case Resource::RESOURCE_TYPE::R_SCENE:
		{
			scene_file_image->AddReference();
			tex = scene_file_image;
		}
			break;
		default:
			tex = (ResourceTexture*) App->resources->GetAndReference(App->importer->texture->checkers);
			break;
		}
		assets_in_folder.emplace((*it), tex);
	}
}

void PanelProject::ManageClicksForItem(std::string file)
{
	if (ImGui::IsItemClicked())
	{
		if (ImGui::IsMouseDoubleClicked(0))
		{
			std::string extension;
			App->fsystem->SplitFilePath(file.c_str(), nullptr, nullptr, &extension);
			std::string dot_extension = ".";
			dot_extension += extension;

			if (dot_extension == SCENE_EXTENSION)
				App->scene->LoadScene(ASSETS_FOLDER + file);
		}
		else
		{
			selected_file = file;

			uint id = App->resources->GetID(ASSETS_FOLDER + file);
			selected_resource = App->resources->Get(id);

			App->scene->selected_gameobject = nullptr;
		}
	}
}