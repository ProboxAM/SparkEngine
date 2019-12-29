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
private:
	enum Mode {
		ALL,
		LOADED,
		NOT_LOADED,
		MESH,
		TEXTURE,
		ANIMATION,
		ANIMATOR,
		BONE,
		TOTAL
	};

	Mode mode = ALL;
};


#endif // !_PANEL_RESOURCES_H_

