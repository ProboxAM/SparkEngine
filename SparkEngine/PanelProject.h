#ifndef _PANEL_PROJECT_H_
#define _PANEL_PROJECT_H_

#include "Panel.h"

class Resource;
class ResourceTexture;

class PanelProject : public Panel
{
public:
	PanelProject(bool active);
	~PanelProject();

	void Draw();
	void DrawFiles();
	void Start();

	std::string selected_item = "";
	Resource* selected_resource = nullptr;

private:
	struct Project_Node {
		std::string folder = "";
		std::string full_path = "";
		Project_Node* parent = nullptr;

		std::vector<std::string> files, directories;
	};

	void DrawPathRecursive(Project_Node* node);
	void ManageClicksForItem(std::string file);
	void ChangeFolder();
	void LoadFileTextures();
	void GetAllFiles();
	void CleanOldFiles();
	void GetNewFiles();
	void CreateTree(std::string path, Project_Node* parent = nullptr);

	int text_size, image_size;
	bool change_folder = false;
	std::string folder_to_change = "";

	std::map<std::string, ResourceTexture*> assets_in_folder;

	std::vector<Project_Node*> project_tree;
	Project_Node* current_node = nullptr;

	ResourceTexture* model_file_image;
	ResourceTexture* scene_file_image;
	ResourceTexture* folder_file_image;
};


#endif // !_PANEL_PROJECT_H_

