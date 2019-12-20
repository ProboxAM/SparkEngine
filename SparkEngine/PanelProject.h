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
	void Start();
	void Reload();
	std::string GetCurrentPath() const;
	void SetItemSize(int img_size, int txt_size);
	void GetItemSize(uint &img_size, uint &txt_size);

	std::string selected_item = "";
	Resource* selected_resource = nullptr;

private:
	struct Project_Node {
		bool model = false;
		std::string folder = "";
		std::string full_path = "";
		Project_Node* parent = nullptr;

		std::vector<std::string> files, directories;
	};

	void DrawFiles();
	void DrawPathRecursive(Project_Node* node);
	void ManageClicksForItem(const std::string &file);
	void ChangeFolder();
	void GetAllFiles();
	void CleanOldFiles();
	void GetNewFiles();
	void CreateTree(const std::string &path, Project_Node* parent = nullptr);
	void DrawResourceNodes(const std::string &file, uint &childs);

	void CleanTree();

	int text_size, image_size;
	bool change_folder = false;
	std::string folder_to_change = "";
	std::vector<std::string> opened_files;

	std::vector<Project_Node*> project_tree;
	Project_Node* current_node = nullptr;
};


#endif // !_PANEL_PROJECT_H_

