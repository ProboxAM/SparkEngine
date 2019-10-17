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
	ImGui::Begin("Scene",NULL, ImGuiWindowFlags_MenuBar);
	ImGui::GetWindowDrawList()->AddImage(
		(void *)App->renderer3D->scene_texture_id,
		ImVec2(0,0),
		ImVec2(0 + App->window->GetWindowWidth(), 0 + App->window->GetWindowHeight()),
		ImVec2(0, 1), 
		ImVec2(1, 0));
	if (ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("Renderer"))
		{
			if (ImGui::Checkbox("Cull Face", &cull_face))
				App->renderer3D->GLEnable(GL_CULL_FACE, cull_face);
			if (ImGui::Checkbox("Lighting", &lighting))
				App->renderer3D->GLEnable(GL_LIGHTING, lighting);
			if (ImGui::Checkbox("Color Material", &color_material))
				App->renderer3D->GLEnable(GL_COLOR_MATERIAL, color_material);
			if (ImGui::Checkbox("Texture 2D", &texture_2D))
				App->renderer3D->GLEnable(GL_TEXTURE_2D, texture_2D);

			ImGui::EndMenu();
		}
		ImGui::Checkbox("Show Grid", &App->scene->show_grid);
		if (ImGui::Checkbox("Wireframe", &wireframe))
			App->renderer3D->SetWireframeMode(wireframe);
		ImGui::EndMenuBar();
	}
	ImGui::End();
}
