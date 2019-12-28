#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "MathGeoLib/Geometry/AABB2D.h"

#include "ResourceAnimatorController.h"

#include "PanelAnimator.h"

#define CHECKBOX_SIZE 50

PanelAnimator::PanelAnimator(bool active):Panel(active)
{
}

PanelAnimator::~PanelAnimator()
{

}


void PanelAnimator::Draw()
{
	ImGui::Begin("Animator", &active);

	if (current_animator) {
		ax::NodeEditor::SetCurrentEditor(current_animator->GetEditorContext());

		ax::NodeEditor::Begin("Animation controller Editor");

		unique_id = 0;
		pin_in_id = 10;
		pin_out_id = 20;
		link_id = 40;

		// Start drawing nodes.
		DrawStates();
		DrawTransitions();

		HandleContextMenu();

		ax::NodeEditor::Suspend();
		ShowNewStatePopup();
		ShowStatePopup();
		ax::NodeEditor::Resume();

		ax::NodeEditor::End();
		ax::NodeEditor::SetCurrentEditor(nullptr);
	}

	ImGui::End();
}

void PanelAnimator::DrawStates()
{
	for (uint i = 0, count = current_animator->GetNumStates(); i < count; ++i)
	{
		// Start drawing nodes.
		ax::NodeEditor::BeginNode(++unique_id);
		current_animator->GetStates()[i]->id = unique_id;

		ImGui::Text(current_animator->GetStates()[i]->GetName().c_str());

		if(current_animator->GetStates()[i]->GetClip())
			ImGui::Text(current_animator->GetStates()[i]->GetClip()->GetName().c_str());
		else
			ImGui::Text("No clip selected");

		ax::NodeEditor::BeginPin(++pin_in_id, ax::NodeEditor::PinKind::Input);
		current_animator->GetStates()[i]->pin_in_id = pin_in_id;
		ImGui::Text("-> In");
		ax::NodeEditor::EndPin();
		ImGui::SameLine();
		ax::NodeEditor::BeginPin(++pin_out_id, ax::NodeEditor::PinKind::Output);
		current_animator->GetStates()[i]->pin_out_id = pin_out_id;
		ImGui::Text("Out ->");
		ax::NodeEditor::EndPin();
		ax::NodeEditor::EndNode();

		LOG("Node created with id: %i pin_in id: %i pin out id: %i", unique_id, pin_in_id, pin_out_id);
	}
}

void PanelAnimator::HandleContextMenu()
{
	ax::NodeEditor::Suspend();

	context_node_id = 0;
	ax::NodeEditor::PinId context_pin_id = 0;
	ax::NodeEditor::LinkId context_link_id = 0;

	if (ax::NodeEditor::ShowBackgroundContextMenu()) {
		ImGui::OpenPopup("States popup");
	}

	if (ax::NodeEditor::ShowNodeContextMenu(&context_node_id)) {
		context_node = current_animator->GetStates()[(uint)context_node_id - 1]->GetName();
		ImGui::OpenPopup("State popup");
	}

	ax::NodeEditor::Resume();	
}

void PanelAnimator::DrawTransitions()
{
	ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 4.0f);
	for (uint i = 0; i < current_animator->GetNumTransitions(); i++)
	{
		State* source = current_animator->FindState(current_animator->GetTransitions()[i]->GetSource()->GetName());
		State* target = current_animator->FindState(current_animator->GetTransitions()[i]->GetTarget()->GetName());

		ax::NodeEditor::Link(++link_id, source->pin_out_id, target->pin_in_id);
		LOG("link with node id: %i pin_out_source %i and node target id %i pin_in_target %i", source->id, source->pin_out_id, target->id, target->pin_in_id);
	}	
	ax::NodeEditor::PopStyleVar(1);
}

void PanelAnimator::ShowStatePopup(){
	if (ImGui::BeginPopup("State popup")) {

		ImGui::Separator();
		char tmp[128];

		snprintf(tmp, 127, current_animator->FindState(context_node)->GetName().c_str());
		if (ImGui::InputText("Name", tmp, 128, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			current_animator->FindState(context_node)->SetName(tmp);
			ImGui::CloseCurrentPopup();
		}

		ImGui::Separator();

		ImGui::Text("Clip list");

		ImGui::Separator();

		for (int i = 0; i < current_animator->GetClips().size(); i++) {
			if (ImGui::Selectable(current_animator->GetClips()[i]->GetName().c_str())) {
				current_animator->FindState(context_node)->SetClip(current_animator->GetClips()[i]);
				ImGui::CloseCurrentPopup();
			}
		}

		if (ImGui::Selectable("Delete"))
		{
			ax::NodeEditor::DeleteNode(ax::NodeEditor::NodeId((context_node_id)));
			current_animator->RemoveState(context_node);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void PanelAnimator::CreateState()
{
	uint aux = 0;
	std::string name = "New state";
	for (; aux < current_animator->GetStates().size(); aux++) {}

	name.append(std::to_string(aux));
	current_animator->AddState(name, nullptr);
}

void PanelAnimator::ShowNewStatePopup()
{
	if (ImGui::BeginPopup("States popup")) {
		if (ImGui::Selectable("NewState")) {
			CreateState();
		}
		ImGui::EndPopup();
	}
}

void PanelAnimator::Start()
{

}

bool PanelAnimator::IsInside(const float2 & pos) const
{
	AABB2D box(float2(screen_pos.x, screen_pos.y), float2(screen_pos.x + w, screen_pos.y + h));
	return math::Contains(box, float3(pos.x, pos.y, 0));
}

void PanelAnimator::SetCurrentResourceAnimatorController(ResourceAnimatorController * animator)
{
	current_animator = animator;
}
