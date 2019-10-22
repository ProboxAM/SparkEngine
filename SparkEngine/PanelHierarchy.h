#ifndef _PANEL_HIERARCHY_H_
#define _PANEL_HIERARCHY_H_

#include "Panel.h"

class ComponentTransform;

class PanelHierarchy :
	public Panel
{
public:
	PanelHierarchy(bool a);
	~PanelHierarchy();

	void Draw();
	void DrawNode(ComponentTransform* ct);

	int node_iterator = -1;
};

#endif // !_PANEL_HIERARCHY_H_

