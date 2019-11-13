#ifndef _PANEL_SCENE_H_
#define _PANEL_SCENE_H_

class ImVec2;
#include "Panel.h"

class PanelScene : public Panel
{
public:
	PanelScene(bool active);
	~PanelScene();
	void Draw();
	void Start();
	bool IsInside(const float2& pos) const;
	ImRect GetRect();

private:
	void GetSizeWithAspectRatio(int current_width, int current_height, int wanted_width, int wanted_height, int& new_width, int& new_height);

private:
	ImVec2 screen_pos;
	ImRect rect;
	float w, h;
};

#endif // !_PANEL_SCENE_H_



