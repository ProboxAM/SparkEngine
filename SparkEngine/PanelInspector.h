#pragma once
#include "Panel.h"
class PanelInspector :
	public Panel
{
public:
	PanelInspector(bool a);
	~PanelInspector();

	void Draw();
private:

	bool vertex_normals_enabled = false;
	bool face_normals_enabled = false;
};

