#pragma once
#include "Panel.h"
class PanelInspector :
	public Panel
{
public:
	PanelInspector(bool a);
	~PanelInspector();

	void Draw();
};

