#ifndef _PANEL_GAME_H_
#define _PANEL_GAME_H_

class ImVec2;

#include "Panel.h"

class PanelGame : public Panel
{
public:
	PanelGame(bool active);
	~PanelGame();
	void Draw();
	void Start();

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

private:
	bool stats = false;
	ImVec2 screen_pos;
	float w, h;
};

#endif // !_PANEL_SCENE_H_
