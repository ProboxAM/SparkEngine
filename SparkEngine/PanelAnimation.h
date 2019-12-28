#ifndef _PANEL_ANIMATION_H_
#define _PANEL_ANIMATION_H_

#include "Panel.h"
class ResourceAnimation;

class PanelAnimation :
	public Panel
{
public:
	PanelAnimation(bool a);
	~PanelAnimation();

	void Draw();

private:
	void DrawAnimationTimeLine(ResourceAnimation * r_anim);
};

#endif // !_PANEL_ANIMATION_H_



