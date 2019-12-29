#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleResources.h"
#include "ModuleInput.h"
#include "MathGeoLib/Geometry/AABB2D.h"

#include "ResourceAnimatorController.h"
#include "ResourceAnimation.h"

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
		pin_in_id = 100;
		pin_out_id = 200;
		link_id = 300;

		// Start drawing nodes.
		DrawStates();
		DrawTransitions();
		HandleDropLink();

		HandleContextMenu();

		ax::NodeEditor::Suspend();
		ShowNewStatePopup();
		ShowStatePopup();
		ShowLinkPopup();
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
			ImGui::Text(current_animator->GetStates()[i]->GetClip()->name.c_str());
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

	if (ax::NodeEditor::ShowLinkContextMenu(&context_link_id)) {
		selected_link_index = (uint)context_link_id;
		selected_link_index = selected_link_index - 300 - 1;
		ImGui::OpenPopup("Link popup");
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

		std::vector<Resource*> clips = App->resources->GetResources(Resource::RESOURCE_TYPE::R_ANIMATION);

		for (int i = 0; i < clips.size(); i++) {
			if (ImGui::Selectable(((ResourceAnimation*)clips[i])->name.c_str())) {
				current_animator->FindState(context_node)->SetClip((ResourceAnimation*)clips[i]);
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::Separator();

		if (ImGui::Selectable("Delete State"))
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

	if (new_node_id != ax::NodeEditor::PinId::Invalid)
	{
		State* target_state = current_animator->GetStates().back();
		current_animator->AddTransition(source_state, target_state, 0);
	}
}

void PanelAnimator::HandleDropLink()
{
	if (ax::NodeEditor::BeginCreate())
	{
		ax::NodeEditor::PinId start_pin_id = 0, end_pin_id = 0;

		if (ax::NodeEditor::QueryNewLink(&start_pin_id, &end_pin_id)) {

			if (start_pin_id && end_pin_id)
			{
				bool start_is_input, end_is_input;

				if ((uint)start_pin_id > 100 && (uint)start_pin_id < 200)
					start_is_input = true;
				else
					start_is_input = false;

				if ((uint)end_pin_id > 100 && (uint)end_pin_id < 200)
					end_is_input = true;
				else
					end_is_input = false;

				State* start_node = current_animator->FindStateFromPinId(start_pin_id.Get());
				State* end_node = current_animator->FindStateFromPinId(end_pin_id.Get());

				if (start_pin_id == end_pin_id)
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				else if (start_is_input && end_is_input)
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				else if (start_node == end_node)
					ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				else {
					if (ax::NodeEditor::AcceptNewItem(ImColor(0, 255, 0), 4.0f))
					{
						if (start_is_input)
							current_animator->AddTransition(end_node, start_node, 0);
						else
							current_animator->AddTransition(start_node, end_node, 0);
					}
				}

			}
		}

		if (ax::NodeEditor::QueryNewNode(&new_node_id))
		{
			bool pin_is_input;

			if ((uint)new_node_id > 100 && (uint)new_node_id < 200)
				pin_is_input = true;
			else
				pin_is_input = false;

			if (!pin_is_input)
			{
				if (ax::NodeEditor::AcceptNewItem())
				{
					source_state = current_animator->FindStateFromPinId((uint)new_node_id);
					ax::NodeEditor::Suspend();
					new_node_pos = ImGui::GetMousePos();
					ImGui::OpenPopup("States popup");
					ax::NodeEditor::Resume();
				}
			}
		}
	}

	ax::NodeEditor::EndCreate();
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

void PanelAnimator::ShowLinkPopup()
{
	if (ImGui::BeginPopup("Link popup")) {

		if (current_animator->GetTransitions()[selected_link_index]->GetTrigger() != -1) {
			std::string trigger_slot_selected = "Current Trigger slot: 0";
			trigger_slot_selected.append(std::to_string(current_animator->GetTransitions()[selected_link_index]->GetTrigger()));
			ImGui::Text(trigger_slot_selected.c_str());
		}else
			ImGui::Text("Select trigger slot");

		ImGui::Separator();
		
		for (int i = 0; i < current_animator->GetTriggers().size(); ++i) {
			std::string trigger_slot = "Trigger_slot 0";
			trigger_slot.append(std::to_string(i));
			if (ImGui::Selectable(trigger_slot.c_str())) {
				current_animator->GetTransitions()[selected_link_index]->SetTrigger((uint)i);
			}
		}

		ImGui::Separator();

		int blend_v = (int)current_animator->GetTransitions()[selected_link_index]->GetBlend();
			
		if (ImGui::InputInt("Blend value: ", &blend_v)) {
			current_animator->GetTransitions()[selected_link_index]->SetBlend((uint)blend_v);
		}

		ImGui::Separator();

		if (ImGui::Selectable("Delete Transition")) {
			current_animator->RemoveTransition(current_animator->GetTransitions()[selected_link_index]->GetSource()->GetName(), current_animator->GetTransitions()[selected_link_index]->GetTarget()->GetName());
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
	if (current_animator)
	{
		current_animator->SaveAsset();
		current_animator->RemoveReference();
	}

	current_animator = animator;
	current_animator->AddReference();
}
