#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"

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
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;;
	node_iterator++;

	ImGuiTreeNodeFlags node_flags = base_flags;

	if (App->scene->selected_gameobject)
	{
		if (App->scene->selected_gameobject == ct->gameobject) {
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}	
	}

	if (ct->GetChildCount() > 0)
	{
		bool node_open = ImGui::TreeNodeEx(ct->gameobject->GetName().c_str(), node_flags);

		SetDragAndDropSource(ct);
		SetDragAndDropTarget(ct);
		SetDragAndDropTargetCustom();

		if (ImGui::IsItemClicked(LEFT_CLICK) || ImGui::IsItemClicked(RIGHT_CLICK)) {
			App->scene->selected_gameobject = ct->gameobject;
			LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
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
		ImGui::TreeNodeEx(ct->gameobject->GetName().c_str(), node_flags);

		SetDragAndDropSource(ct);
		SetDragAndDropTarget(ct);

		if (ImGui::IsItemClicked(LEFT_CLICK) || ImGui::IsItemClicked(RIGHT_CLICK)) {
			App->scene->selected_gameobject = ct->gameobject;
			LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
		}

		OnNodeRightClick();

	}

}

void PanelHierarchy::SetDragAndDropTarget(ComponentTransform * target)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
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
		ImGui::SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &App->scene->selected_gameobject->transform, sizeof(ComponentTransform));
		ImGui::EndDragDropSource();
	}
}

void PanelHierarchy::SetDragAndDropTargetCustom()
{
	if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->Rect(), (ImGuiID)"Hierarchy"))
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
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
	if (ImGui::BeginPopupContextItem(nullptr, RIGHT_CLICK)) {
		if (ImGui::Selectable("Create Empty")) {
			App->scene->CreateGameObject(App->scene->selected_gameobject);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Selectable("Delete")) {
			App->scene->selected_gameobject->transform->GetParent()->RemoveChild(App->scene->selected_gameobject->transform);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}


