#ifndef _PANEL_SCENE_H_
#define _PANEL_SCENE_H_

#include "Panel.h"
class PanelScene : public Panel
{
public:
	PanelScene(bool active);
	~PanelScene();
	void Draw();
	void Start();
	bool IsInside(const float2& pos) const;

private:
	ImVec2 screen_pos;
	float w, h;
};

#endif // !_PANEL_SCENE_H_



