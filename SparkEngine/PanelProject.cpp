#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleImporter.h"
#include "ModuleResources.h"
#include "TextureImporter.h"
#include "ResourceTexture.h"
#include "ResourceModel.h"
#include "ResourceMesh.h"

#include "PanelProject.h"

PanelProject::PanelProject(bool active): Panel(active)
{
}


PanelProject::~PanelProject()
{
}

void PanelProject::Start()
{
	CreateTree(ASSETS_FOLDER);
	current_node = project_tree[0];

	GetAllFiles();
}

void PanelProject::Reload()
{
	std::string current_folder = current_node->folder;
	CleanOldFiles();
	CleanTree();

	CreateTree(ASSETS_FOLDER);
	current_node = project_tree[0];
	for (std::vector<Project_Node*>::iterator it = project_tree.begin(); it != project_tree.end(); ++it)
	{
		if ((*it)->folder == current_folder)
		{
			current_node = (*it);
			break;
		}
	}
	GetAllFiles();
}

std::string PanelProject::GetCurrentPath() const
{
	return current_node->full_path;
}

void PanelProject::SetItemSize(int img_size, int txt_size)
{
	image_size = img_size;
	text_size = txt_size;
}

void PanelProject::GetItemSize(uint & img_size, uint & txt_size)
{
	img_size = image_size;
	txt_size = text_size;
}

void PanelProject::CreateTree(const std::string &path, Project_Node* parent)
{
	Project_Node* node = new Project_Node();
	node->parent = parent;
	node->full_path = path;
	App->fsystem->GetFolder(path.c_str(), &node->folder);
	App->fsystem->DiscoverFiles(path.c_str(), node->files, node->directories, "meta");

	project_tree.push_back(node);
	for each (std::string folder in node->directories)
		CreateTree(path + folder + "/", node);
}

void PanelProject::CleanTree()
{
	for (std::vector<Project_Node*>::iterator it = project_tree.begin(); it != project_tree.end();)
	{
		delete (*it);
		it = project_tree.erase(it);
	}
	project_tree.clear();
}

void PanelProject::GetAllFiles()
{
	CleanOldFiles();
	GetNewFiles();
}

void PanelProject::CleanOldFiles()
{
	for (std::vector<std::string>::iterator it = current_node->files.begin(); it != current_node->files.end(); ++it)
	{
		uint id = App->resources->GetID(current_node->full_path + (*it));
		if (id > 0)
			App->resources->Get(id)->RemoveReference();
	}

	selected_item = "";
	selected_resource = nullptr;
	folder_to_change = "";
}

void PanelProject::GetNewFiles()
{
	//Add reference to textures inside selected folder as they are being used here. If folder changes unreference texture
	for (std::vector<std::string>::iterator it = current_node->files.begin(); it != current_node->files.end(); ++it)
	{
		std::string extension;
		App->fsystem->SplitFilePath((*it).c_str(), nullptr, nullptr, &extension);
		uint id = App->resources->GetID(current_node->full_path + (*it));
		if(id > 0)
			App->resources->GetAndReference(id);
	}
}

void PanelProject::Draw()
{
	ImGui::Begin("Project", &active);

	DrawFiles();

	ImGui::End();
}

void PanelProject::DrawFiles()
{
	if(change_folder)
		ChangeFolder();

	DrawPathRecursive(current_node);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::GetWindowWidth() * 0.1f);
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
	ImGui::SliderInt(" ", &image_size, 25, 100, " ");
	ImGui::Separator();

	uint childs = 1;
	for (std::vector<std::string>::iterator it = current_node->directories.begin(); it != current_node->directories.end(); ++it)
	{
		ImGui::SameLine();
		ImGui::BeginChild(childs, { (float)image_size, (float)image_size + text_size }, false, ImGuiWindowFlags_NoScrollbar);
		ImGui::Image((ImTextureID)App->editor->atlas->buffer_id, ImVec2(image_size, image_size),
			ImVec2((float)App->editor->icon_size/App->editor->atlas->width, (float)App->editor->icon_size /App->editor->atlas->height),
			ImVec2((float)App->editor->icon_size*2/App->editor->atlas->width, 0));
		ManageClicksForItem((*it));
		ImGui::Text((*it).c_str());
		ManageClicksForItem((*it));
		if (selected_item == *it)
		{
			ImGui::SetCursorPos(ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 75, 255, 55));
			ImGui::BeginChild("Selected", ImVec2(image_size, image_size + text_size), true, ImGuiWindowFlags_NoInputs);
			ImGui::EndChild();
			ImGui::PopStyleColor();
		}

		ImGui::EndChild();
		childs++;
	}

	for (std::vector<std::string>::iterator it = current_node->files.begin(); it != current_node->files.end(); ++it)
	{
		ImGui::SameLine();
		ImGui::BeginChild(childs, { (float)image_size, (float)image_size + text_size }, false, ImGuiWindowFlags_NoScrollbar);

		uint id = App->resources->GetID(current_node->full_path + (*it));
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("ASSET", &id, sizeof(uint));
			ImGui::EndDragDropSource();
		}
		std::string extension;
		App->fsystem->SplitFilePath((*it).c_str(), nullptr, nullptr, &extension);
		switch (App->resources->GetTypeFromExtension(extension))
		{
		case Resource::RESOURCE_TYPE::R_SCENE:
			ImGui::Image((ImTextureID)App->editor->atlas->buffer_id, ImVec2(image_size, image_size),
				ImVec2(0 / App->editor->atlas->width, (float)App->editor->icon_size / App->editor->atlas->height),
				ImVec2((float)App->editor->icon_size / App->editor->atlas->width, 0));
			break;
		case Resource::RESOURCE_TYPE::R_MODEL:
			ImGui::Image((ImTextureID)App->editor->atlas->buffer_id, ImVec2(image_size, image_size),
				ImVec2((float)256 / App->editor->atlas->width, (float)App->editor->icon_size*2 / App->editor->atlas->height),
				ImVec2(1.0f, (float)App->editor->icon_size / App->editor->atlas->height));
			break;
		case Resource::RESOURCE_TYPE::R_TEXTURE:
			uint id = App->resources->GetID(current_node->full_path + (*it));
			ImGui::Image((ImTextureID)((ResourceTexture*)App->resources->Get(id))->buffer_id, ImVec2(image_size, image_size), ImVec2(0, 1), ImVec2(1, 0));
			break;
		}
		ManageClicksForItem((*it));

		ImGui::Text((*it).c_str());
		ManageClicksForItem((*it));

		if (selected_item == (*it))
		{
			ImGui::SetCursorPos(ImVec2(0,0));
			ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 75, 255, 55));
			ImGui::BeginChild("Selected", ImVec2(image_size, image_size+text_size), true, ImGuiWindowFlags_NoInputs);
			ImGui::EndChild();
			ImGui::PopStyleColor();
		}

		ImGui::EndChild();
		childs++;

		Resource::RESOURCE_TYPE type = App->resources->GetTypeFromExtension(extension);
		if (type == Resource::RESOURCE_TYPE::R_MODEL)
		{
			for (std::vector<std::string>::iterator open_it = opened_files.begin(); open_it != opened_files.end(); ++open_it)
			{
				if ((*it) == (*open_it))
				{
					uint id = App->resources->GetID(current_node->full_path + (*it));
					ResourceModel::ModelMetaFile* meta = (ResourceModel::ModelMetaFile*)App->resources->Get(id)->meta;
					for each (uint mesh_id in meta->meshes)
					{
						if (mesh_id == 0)
							continue;

						ImGui::SameLine();
						ImGui::BeginChild(childs, { (float)image_size, (float)image_size + text_size }, false, ImGuiWindowFlags_NoScrollbar);

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
						{
							ImGui::SetDragDropPayload("ASSET", &mesh_id, sizeof(uint));
							ImGui::EndDragDropSource();
						}

						ImGui::Image((ImTextureID)App->editor->atlas->buffer_id, ImVec2(image_size, image_size),
							ImVec2((float)256 / App->editor->atlas->width, (float)App->editor->icon_size * 2 / App->editor->atlas->height),
							ImVec2(1.0f, (float)App->editor->icon_size / App->editor->atlas->height));
	
						ImGui::Text(std::to_string(mesh_id).c_str());

						ImGui::EndChild();
						childs++;
					}
				}
			}
		}
	}
}

void PanelProject::DrawPathRecursive(Project_Node* node)
{
	if (node->parent)
		DrawPathRecursive(node->parent);
	ImGui::SameLine();
	ImGui::Text(node->folder.c_str());
	if (ImGui::IsItemClicked() && current_node->folder != node->folder)
	{
		folder_to_change = node->folder;
		change_folder = true;
	}
}

void PanelProject::ManageClicksForItem(const std::string &item)
{
	if (ImGui::IsItemClicked())
	{
		if (ImGui::IsMouseDoubleClicked(0))
		{
			std::string extension;
			App->fsystem->SplitFilePath(item.c_str(), nullptr, nullptr, &extension);
			if (!extension.empty())
			{
				Resource::RESOURCE_TYPE type = App->resources->GetTypeFromExtension(extension);

				if (type == Resource::RESOURCE_TYPE::R_SCENE)
					App->scene->LoadScene(current_node->full_path + item);
				else if (type == Resource::RESOURCE_TYPE::R_MODEL)
				{
					for (std::vector<std::string>::iterator it = opened_files.begin(); it != opened_files.end(); ++it)
					{
						if ((*it) == item)
						{
							opened_files.erase(it);
							return;
						}
					}
					opened_files.push_back(item);
				}
			}
			else
			{
				folder_to_change = item;
				change_folder = true;
			}
		}
		else
		{
			selected_item = item;

			uint id = App->resources->GetID(current_node->full_path + item);
			selected_resource = App->resources->Get(id);

			App->scene->selected_gameobject = nullptr;
		}
	}
}

void PanelProject::ChangeFolder()
{
	for (std::vector<Project_Node*>::iterator it = project_tree.begin(); it != project_tree.end(); ++it)
	{
		if ((*it)->folder == folder_to_change)
		{
			CleanOldFiles();
			current_node = (*it);
			GetNewFiles();
		}
	}
	change_folder = false;
}