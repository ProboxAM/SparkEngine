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

	std::string selected_file = "";
	Resource* selected_resource = nullptr;

private:
	void ManageClicksForItem(std::string file);
	uint text_size, image_size;
	std::map<std::string, ResourceTexture*> assets_in_folder;

	ResourceTexture* model_file_image;
	ResourceTexture* scene_file_image;
};


#endif // !_PANEL_PROJECT_H_

