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
	void DrawTransitions();
	void HandleContextMenu();
	void ShowStatePopup();
	void CreateState();
	void ShowNewStatePopup();
	void Start();
	bool IsInside(const float2& pos) const;

public:
	ResourceAnimatorController* current_animator = nullptr;

	void SetCurrentResourceAnimatorController(ResourceAnimatorController* animator);

private:
	bool stats = false;
	ImVec2 screen_pos;
	float w, h;
	bool clip_select = false;
	int pin_in_id = 10;
	int pin_out_id = 20;
	int link_id = 40;
	int unique_id = 1;
	ImVec2 new_node_pos;

	std::string context_node;
	ax::NodeEditor::NodeId context_node_id;
};

#endif // !__PANEL_ANIMATOR_H_

