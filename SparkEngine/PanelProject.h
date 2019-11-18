#ifndef _PANEL_PROJECT_H_
#define _PANEL_PROJECT_H_

#include "Panel.h"

class Resource;

class PanelProject : public Panel
{
public:
	PanelProject(bool active);
	~PanelProject();

	void Draw();
	void DrawFiles();


	
	std::string selected_file = "";
	Resource* selected_resource = nullptr;

private:
	void ManageClicksForItem(std::string file);
	uint text_size, image_size;
};


#endif // !_PANEL_PROJECT_H_

