#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Geometry\AABB2D.h"
#include "glew/glew.h"

#include "PanelScene.h"


PanelScene::PanelScene(bool active): Panel(active)
{
}


PanelScene::~PanelScene()
{
}

void PanelScene::Draw()
{
	ImGui::Begin("Scene", NULL, ImGuiWindowFlags_MenuBar);
	w = ImGui::GetWindowWidth();
	h = ImGui::GetWindowHeight();
	screen_pos = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddImage(
		(void *)App->renderer3D->scene_texture_id,
		screen_pos,
		ImVec2(screen_pos.x + w, screen_pos.y + h),
		ImVec2(0, 1), 
		ImVec2(1, 0));
	if (ImGui::BeginMenuBar())
	{
		bool lighting = App->renderer3D->IsEnabled(GL_LIGHTING);
		if (ImGui::Checkbox("Lighting", &lighting))
			App->renderer3D->GLEnable(GL_LIGHTING, lighting);
		ImGui::Checkbox("Show Grid", &App->scene->show_grid);
		bool wireframe = App->renderer3D->IsWireframeEnabled();
		if (ImGui::Checkbox("Wireframe", &wireframe))
			App->renderer3D->SetWireframeMode(wireframe);
		ImGui::EndMenuBar();
	}
	ImGui::End();
}

void PanelScene::Start()
{
}

bool PanelScene::IsInside(const float2& pos) const
{
	AABB2D box(float2(screen_pos.x, screen_pos.y), float2(screen_pos.x + w, screen_pos.y + h));
	return math::Contains(box, float3(pos.x, pos.y, 0));
}
