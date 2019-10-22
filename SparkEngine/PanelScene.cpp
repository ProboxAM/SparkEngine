#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "MathGeoLib\Math\float3.h"
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
	float w, h;
	w = ImGui::GetWindowWidth();
	h = ImGui::GetWindowHeight();
	ImGui::GetWindowDrawList()->AddImage(
		(void *)App->renderer3D->scene_texture_id,
		ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y),
		ImVec2(ImGui::GetCursorScreenPos().x + w, ImGui::GetCursorScreenPos().y + h),
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
