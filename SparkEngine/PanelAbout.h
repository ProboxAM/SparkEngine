#ifndef _PANEL_ABOUT_H_
#define _PANEL_ABOUT_H_

#include "Panel.h"
class PanelAbout :
	public Panel
{
public:
	PanelAbout(bool a);
	~PanelAbout();

	void Draw();
};

#endif // !_PANEL_ABOUT_H_