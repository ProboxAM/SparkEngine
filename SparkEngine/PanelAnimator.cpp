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
	}

	ImGui::End();
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
