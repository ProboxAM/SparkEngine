#ifndef _PANEL_SCENE_H_
#define _PANEL_SCENE_H_

class ImVec2;
#include "Panel.h"
#include "ImGuizmo.h"


class PanelScene : public Panel
{
public:
	PanelScene(bool active);
	~PanelScene();
	void Draw();
	void Start();
	bool IsInside(const float2& pos) const;

	void GetScreenPos(float &x, float &y) {
		x = screen_pos.x;
		y = screen_pos.y;
	}

	float GetScreenWidth() {
		return w;
	}

	float GetScreenHeight() {
		return h;
	}

	int image_w, image_h; 

private:
	void GetSizeWithAspectRatio(int current_width, int current_height, int wanted_width, int wanted_height, int& new_width, int& new_height);
	void DrawTransformGuizmo();

private:
	ImVec2 screen_pos;
	float w, h;
};

#endif // !_PANEL_SCENE_H_



