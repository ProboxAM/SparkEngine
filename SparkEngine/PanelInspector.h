#ifndef _PANEL_INSPECTOR_H_
#define _PANEL_INSPECTOR_H_

#include "Panel.h"
class PanelInspector :
	public Panel
{
public:
	PanelInspector(bool a);
	~PanelInspector();

	void Draw();
private:
};

#endif // !_PANEL_INSPECTOR_H_

