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
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	static int selection_mask = (1 << 2);
	static int node_selected = -1;
	node_iterator++;

	ImGuiTreeNodeFlags node_flags = base_flags;
	const bool is_selected = (selection_mask & (1 << node_iterator)) != 0;
	if(is_selected)
		node_flags |= ImGuiTreeNodeFlags_Selected;
	if (ct->GetChildCount() > 0)
	{
		bool node_open = ImGui::TreeNodeEx(ct->gameobject->GetName().c_str(), node_flags);

		SetDragAndDropSource(ct);
		SetDragAndDropTarget(ct);

		if (ImGui::IsItemClicked()) {
			node_selected = node_iterator;
			App->scene->selected_gameobject = ct->gameobject;
			LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
		}
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

		if (ImGui::IsItemClicked()) {
			node_selected = node_iterator;
			App->scene->selected_gameobject = ct->gameobject;
			LOG("Current selected object: %s", App->scene->selected_gameobject->GetName().c_str());
		}
	}

	if (node_selected != -1)
	{
		// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= (1 << node_selected);          // CTRL+click to toggle
		else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
			selection_mask = (1 << node_selected);           // Click to single-select
	}
}

void PanelHierarchy::SetDragAndDropTarget(ComponentTransform * target)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
		{
			GameObject* payload_n = *(GameObject**)payload->Data;

			if (!payload_n->transform->IsChild(target))
			{
				math::float4x4 globalMatrix = payload_n->transform->GetTransformMatrix();
				payload_n->transform->GetParent()->DestroyChild(payload_n->transform);
				target->AddChild(payload_n->transform);

				payload_n->transform->SetParent(target);
				payload_n->transform->UpdateTransformMatrix();
			}
		}
		ImGui::EndDragDropTarget();
	}

}

void PanelHierarchy::SetDragAndDropSource(ComponentTransform * target)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &App->scene->selected_gameobject, sizeof(GameObject));
		ImGui::EndDragDropSource();
	}
}


