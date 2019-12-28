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
	void CreateState();
	void HandleDropLink();
	void ShowStatePopup();
	void ShowNewStatePopup();
	void ShowLinkPopup();
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
	int pin_in_id = 100;
	int pin_out_id = 200;
	int link_id = 300;
	int unique_id = 1;
	ImVec2 new_node_pos;

	std::string context_node;
	ax::NodeEditor::NodeId context_node_id;
	uint selected_link_index = 0;

	//creation management
	ax::NodeEditor::PinId new_node_id = 0;
	State* source_state = nullptr;
};

#endif // !__PANEL_ANIMATOR_H_

