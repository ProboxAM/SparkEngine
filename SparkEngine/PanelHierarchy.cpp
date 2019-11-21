#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "PanelProject.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "PanelHierarchy.h"
PanelHierarchy::PanelHierarchy(bool a) : Panel(a)
{

}

PanelHierarchy::~PanelHierarchy()
{
}

void PanelHierarchy::Draw()
{
	ImGui::Begin("Hierarchy", &active);

	node_iterator = 0;

	for (int i = 0; i < App->scene->root->transform->GetChildCount(); i++)
	{
		DrawNode(App->scene->root->transform->GetChildren()[i]);
	}

	ImGui::End();
}

void PanelHierarchy::DrawNode(ComponentTransform * ct)
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	node_iterator++;

	ImGuiTreeNodeFlags node_flags = base_flags;


	if (App->scene->selected_gameobject)
	{
		if (App->scene->selected_gameobject == ct->gameobject) {
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (ct->IsChild(App->scene->selected_gameobject->transform) && App->scene->user_selected_GO)ImGui::SetNextTreeNodeOpen(true);
		if (ct->IsDirectChild(App->scene->selected_gameobject->transform) && App->scene->user_selected_GO) App->scene->user_selected_GO = false;
	}

	if (ct->GetChildCount() > 0)
	{
		ImGui::PushID(node_iterator);
		bool node_open = ImGui::TreeNodeEx(ct->gameobject->GetName().c_str(), node_flags);
		ImGui::PopID();

		SetDragAndDropSource(ct);
		SetDragAndDropTarget(ct);
		SetDragAndDropTargetCustom();

		if (ImGui::IsItemClicked(LEFT_CLICK) || ImGui::IsItemClicked(RIGHT_CLICK) && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
		{
			App->scene->selected_gameobject = ct->gameobject;
			node_flags |= ImGuiTreeNodeFlags_Selected;

			PanelProject* panel_project = (PanelProject*)App->editor->GetPanel(Panel_Type::PROJECT);
			panel_project->selected_item = "";
			panel_project->selected_resource = nullptr;
		}

		OnNodeRightClick();

		if (node_open) {
			for (int i = 0; i < ct->GetChildCount(); i++)
			{
				DrawNode(ct->GetChildren()[i]);
			}
			ImGui::TreePop();
		}

	}
	else {
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
		ImGui::PushID(node_iterator);
		ImGui::TreeNodeEx(ct->gameobject->GetName().c_str(), node_flags);
		ImGui::PopID();

		SetDragAndDropSource(ct);
		SetDragAndDropTarget(ct);

		if (ImGui::IsItemClicked(LEFT_CLICK) || ImGui::IsItemClicked(RIGHT_CLICK) && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing()) {
			App->scene->selected_gameobject = ct->gameobject;
			node_flags |= ImGuiTreeNodeFlags_Selected;

			PanelProject* panel_project = (PanelProject*)App->editor->GetPanel(Panel_Type::PROJECT);
			panel_project->selected_item = "";
			panel_project->selected_resource = nullptr;
		}

		OnNodeRightClick();

	}
}

void PanelHierarchy::SetDragAndDropTarget(ComponentTransform * target)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT"))
		{
			ComponentTransform* payload_n = *(ComponentTransform**)payload->Data;

			if (!payload_n->IsChild(target) && target != payload_n)
			{
				payload_n->GetParent()->RemoveChild(payload_n);
				target->AddChild(payload_n);

				payload_n->SetParent(target);
				payload_n->UpdateTransformMatrix();
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void PanelHierarchy::SetDragAndDropSource(ComponentTransform * target)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("GameObject", &App->scene->selected_gameobject->transform, sizeof(ComponentTransform));
		ImGui::EndDragDropSource();
	}
}

void PanelHierarchy::SetDragAndDropTargetCustom()
{
	if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->Rect(), (ImGuiID)"HIERARCHY"))
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT"))
		{
			App->scene->selected_gameobject->transform->GetParent()->RemoveChild(App->scene->selected_gameobject->transform);
			App->scene->root->transform->AddChild(App->scene->selected_gameobject->transform);

			App->scene->selected_gameobject->transform->SetParent(App->scene->root->transform);
			App->scene->selected_gameobject->transform->UpdateTransformMatrix();
		}
		ImGui::EndDragDropTarget();
	}
}

void PanelHierarchy::OnNodeRightClick()
{
	if ((ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
	{
		if (ImGui::BeginPopupContextItem(nullptr, RIGHT_CLICK)) {
			if (ImGui::Selectable("Create Empty")) {
				App->scene->CreateGameObject(App->scene->selected_gameobject);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Selectable("Delete")) {
				App->scene->DeleteGameObject(App->scene->selected_gameobject);
				App->scene->selected_gameobject = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}


