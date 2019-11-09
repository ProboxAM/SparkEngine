#ifndef _PANEL_PROJECT_H_
#define _PANEL_PROJECT_H_

#include "Panel.h"

struct ProjectNode
{
	std::string name;
	ProjectNode* parent;
	std::vector<ProjectNode*> childs;
};

class PanelProject :
	public Panel
{
public:
	PanelProject(bool active);
	~PanelProject();

	void Draw();
	void DrawFiles();
	std::string FitTextToImage(std::string text);

private:
	uint columns, image_size, spacing, offset;
};


#endif // !_PANEL_PROJECT_H_

