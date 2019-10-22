#ifndef _PANEL_HIERARCHY_H_
#define _PANEL_HIERARCHY_H_

#include "Panel.h"
class PanelHierarchy :
	public Panel
{
public:
	PanelHierarchy(bool a);
	~PanelHierarchy();

	void Draw();
};

#endif // !_PANEL_HIERARCHY_H_

