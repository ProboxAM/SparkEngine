#ifndef _PANEL_RESOURCES_H_
#define _PANEL_RESOURCES_H_

#include "Panel.h"
class PanelResources :
	public Panel
{
public:
	PanelResources(bool active);
	~PanelResources();

	void Draw();
};


#endif // !_PANEL_RESOURCES_H_

