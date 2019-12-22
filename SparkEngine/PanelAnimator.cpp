#include "Application.h"
#include "ModuleWindow.h"

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

		ax::NodeEditor::Begin("ResourceAnimatorControllerName");

		int uniqueId = 1;

		// Start drawing nodes.
		DrawStates();

		ax::NodeEditor::End();
		ax::NodeEditor::SetCurrentEditor(nullptr);
	}

	ImGui::End();
}

void PanelAnimator::DrawStates()
{
	for (uint i = 0, count = current_animator->GetNumStates(); i < count; ++i)
	{
		int uniqueId = 1;

		// Start drawing nodes.
		ax::NodeEditor::BeginNode(uniqueId++);
		ImGui::Text("Node A");
		ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
		ImGui::Text("-> In");
		ax::NodeEditor::EndPin();
		ImGui::SameLine();
		ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
		ImGui::Text("Out ->");
		ax::NodeEditor::EndPin();
		ax::NodeEditor::EndNode();

		ax::NodeEditor::End();
		/*ImVec2 pos = ax::NodeEditor::GetNodePosition(i);*/

		//ImDrawList* drawList = ax::NodeEditor::GetNodeBackgroundDrawList(i * 3 + 1);

		//drawList->AddLine(
		//	ImGui::GetCursorScreenPos(),
		//	{ ImGui::GetCursorScreenPos().x + size.x - 16.0f, ImGui::GetCursorScreenPos().y },
		//	IM_COL32(255, 255, 0, 255), 1.0f);


		//ImGui::PopStyleVar();

		//ImGui::Dummy(ImVec2(96.0, 8.0));
		//ImGui::BulletText("Clip: %s", current_animator->GetStates()[i].GetName().c_str());
		//if (i == current_animator->GetDefaultNode())
		//{
		//	ImGui::BulletText("Entry State");
		//}

		//ImGui::Dummy(ImVec2(96.0, 8.0));

		//drawList->AddLine(
		//	ImGui::GetCursorScreenPos(),
		//	{ ImGui::GetCursorScreenPos().x + size.x - 16.0f, ImGui::GetCursorScreenPos().y },
		//	IM_COL32(255, 255, 0, 255), 1.0f);

		//ImGui::Dummy(ImVec2(64.0, 8.0));

		//// In Pin
		//ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowSize, 8.0f);
		//ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowWidth, 8.0f);
		//ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinRadius, 10.0f);
		//ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
		//ax::NodeEditor::BeginPin(i * 3 + 2, ax::NodeEditor::PinKind::Input);
		//ImGui::Text("In");
		//ax::NodeEditor::EndPin();
		//ax::NodeEditor::PopStyleVar(4);

		//// Out Pin
		//ImGui::SameLine(size.x - 40);
		//ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowSize, 0.0f);
		//ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowWidth, 0.0f);
		//ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
		//ax::NodeEditor::BeginPin(i * 3 + 3, ax::NodeEditor::PinKind::Output);
		//ImGui::Text("Out");

		//ax::NodeEditor::EndPin();

		/*ax::NodeEditor::EndNode();*/

		//ax::NodeEditor::PopStyleVar(3);
		//ax::NodeEditor::PopStyleColor(2);
	}
}

void PanelAnimator::Start()
{

}

bool PanelAnimator::IsInside(const float2 & pos) const
{
	return false;
}

void PanelAnimator::SetCurrentResourceAnimatorController(ResourceAnimatorController * animator)
{
	current_animator = animator;
}
