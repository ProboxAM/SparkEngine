#pragma once
#include "Panel.h"
class PanelHierarchy :
	public Panel
{
public:
	PanelHierarchy(bool a);
	~PanelHierarchy();

	void Draw();
};

