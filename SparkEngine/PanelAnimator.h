#ifndef _PANEL_ANIMATOR_H_
#define _PANEL_ANIMATOR_H_

class ImVec2;

#include "Panel.h"
#include "MathGeoLib\Math\MathAll.h"
#include "NodeEditor/Include/imgui_node_editor.h"
#include "ComponentAnimator.h"
#include "ResourceAnimatorController.h"

class PanelAnimator :
	public Panel
{
public:
	PanelAnimator(bool active);
	~PanelAnimator();

	void Draw();
	void DrawStates();
	void Start();
	bool IsInside(const float2& pos) const;

public:
	ResourceAnimatorController* current_animator = nullptr;

	void SetCurrentResourceAnimatorController(ResourceAnimatorController* animator);

private:
	bool stats = false;
	ImVec2 screen_pos;
	float w, h;
};

#endif // !__PANEL_ANIMATOR_H_

