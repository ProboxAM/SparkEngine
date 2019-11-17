#ifndef _PANEL_INSPECTOR_H_
#define _PANEL_INSPECTOR_H_

#include "Panel.h"

class Resource;

class PanelInspector :
	public Panel
{
public:
	PanelInspector(bool a);
	~PanelInspector();

	void Draw();
	
private:
	void ShowTextureImportSettings(Resource* res);
	void ShowModelImportSettings(Resource* res);
};

#endif // !_PANEL_INSPECTOR_H_

