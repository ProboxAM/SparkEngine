#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Geometry\AABB2D.h"
#include "ImGui/imgui.h"
#include "glew/glew.h"

#include "PanelGame.h"

#define CHECKBOX_SIZE 50



PanelGame::PanelGame(bool active) : Panel(active)
{

}

PanelGame::~PanelGame()
{
}

void PanelGame::Draw()
{
	ImGui::Begin("Game", &active, ImGuiWindowFlags_MenuBar);

	w = ImGui::GetWindowWidth();
	h = ImGui::GetWindowHeight() - CHECKBOX_SIZE;
	GetSizeWithAspectRatio(App->window->GetWindowWidth(), App->window->GetWindowHeight(), w, h, image_w, image_h);
	screen_pos = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + (h * 0.5f - image_h * 0.5f));
	ImGui::GetWindowDrawList()->AddImage(
		(void *)App->renderer3D->game_texture_id,
		screen_pos,
		ImVec2(screen_pos.x + image_w, screen_pos.y + image_h),
		ImVec2(0, 1),
		ImVec2(1, 0));

	if (ImGui::BeginMenuBar())
	{
		bool lighting = App->renderer3D->IsEnabled(GL_LIGHTING);
		if (ImGui::Checkbox("Lighting", &lighting))
			App->renderer3D->GLEnable(GL_LIGHTING, lighting);
		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void PanelGame::Start()
{
}

void PanelGame::GetSizeWithAspectRatio(int current_width, int current_height, int wanted_width, int wanted_height, int & new_width, int & new_height)
{
	float scale_height = (float)wanted_height / (float)current_height;
	float scale_width = (float)wanted_width / (float)current_width;

	float scale = (std::min)(scale_height, scale_width);

	new_width = current_width * scale;
	new_height = current_height * scale;
}
