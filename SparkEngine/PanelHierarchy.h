#pragma once
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

