#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "PanelProject.h"

#include "GameObject.h"
#include "ComponentAnimator.h"

#include "ResourceAnimation.h"
#include "ResourceAnimatorController.h"

#include "PanelAnimation.h"



PanelAnimation::PanelAnimation(bool a):Panel(a)
{
}


PanelAnimation::~PanelAnimation()
{
}

void PanelAnimation::Draw()
{
	ImGui::Begin("Animation", &active, ImGuiWindowFlags_MenuBar);
	if (App->scene->selected_gameobject && App->scene->selected_gameobject != App->scene->root)
	{
		ComponentAnimator* c_anim = (ComponentAnimator*)App->scene->selected_gameobject->GetComponent(COMPONENT_TYPE::ANIMATOR);
		if (c_anim)
		{
			ResourceAnimatorController* r_anim_controller = c_anim->GetResourceAnimatorController();
			if (r_anim_controller)
			{
				//DRAW SELECTED ANIMATION IN CONTROLLER
			}
		}
	}
	else if (((PanelProject*)App->editor->GetPanel(Panel_Type::PROJECT))->selected_resource)
	{
		Resource* r = ((PanelProject*)App->editor->GetPanel(Panel_Type::PROJECT))->selected_resource;
		if (r->GetType() == Resource::RESOURCE_TYPE::R_ANIMATION)
		{
			DrawAnimationTimeLine((ResourceAnimation*)r);
		}
	}

	ImGui::End();
}

void PanelAnimation::DrawAnimationTimeLine(ResourceAnimation* r_anim)
{
	if (ImGui::BeginMenuBar())
	{
		float start_x = ImGui::GetCursorPosX() + ImGui::GetWindowWidth() * 0.3F + 12;
		ImGui::Text(r_anim->name.c_str());
		for (int i = 0; i <= r_anim->max_tick; i++)
		{
			ImGui::SetCursorPosX(start_x + (i*30));
			ImGui::Text(std::to_string(i).c_str());
		}
		ImGui::EndMenuBar();
	}

	ImGui::BeginChild("Channels", ImVec2(ImGui::GetWindowWidth() * 0.3F, ImGui::GetWindowHeight()), true);
	for (int i = 0; i < r_anim->num_channels; i++)
	{
		ImGui::Text(std::string(r_anim->channels[i].name + ": Position").c_str());
		ImGui::Text(std::string(r_anim->channels[i].name + ": Rotation").c_str());
		ImGui::Text(std::string(r_anim->channels[i].name + ": Scale").c_str());
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Keys", ImVec2(ImGui::GetWindowWidth() * 0.7F, ImGui::GetWindowHeight()), true, ImGuiWindowFlags_HorizontalScrollbar);
	float start_x, start_y;
	start_x = ImGui::GetCursorScreenPos().x;
	start_y = ImGui::GetCursorScreenPos().y;
	for (int i = 0; i <= r_anim->max_tick; i++)
	{
		ImGui::GetWindowDrawList()->AddLine(ImVec2(start_x + (i * 30), start_y), ImVec2(start_x + (i * 30), start_y + ImGui::GetWindowHeight()), IM_COL32(100, 100, 100, 255), 1.0f);
	}
	start_y += 5;
	for (int i = 0; i < r_anim->num_channels; i++)
	{
		for (int j = r_anim->start_tick; j <= r_anim->end_tick; j++)
		{
			ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(start_x + (r_anim->channels[i].position_keys[j].time * 30), start_y + (i * 17)), 5.0f, ImColor(0.0f, 0.0f, 1.0f, 0.5f));
		}
		for (int j = r_anim->start_tick; j <= r_anim->end_tick; j++)
		{
			ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(start_x + (r_anim->channels[i].rotation_keys[j].time * 30), start_y + ((i * 17) * 2)), 5.0f, ImColor(0.0f, 0.0f, 1.0f, 0.5f));
		}
		for (int j = r_anim->start_tick; j <= r_anim->end_tick; j++)
		{
			ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(start_x + (r_anim->channels[i].scale_keys[j].time * 30), start_y + ((i * 17) * 3)), 5.0f, ImColor(0.0f, 0.0f, 1.0f, 0.5f));
		}
	}

	ImGui::EndChild();
}
