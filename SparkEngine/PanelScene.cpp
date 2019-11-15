#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
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
	ImGui::Begin("Scene", &active, ImGuiWindowFlags_MenuBar);
	w = ImGui::GetWindowWidth();
	h = ImGui::GetWindowHeight();
	screen_pos = ImGui::GetCursorScreenPos();
	GetSizeWithAspectRatio(App->window->GetWindowWidth(), App->window->GetWindowHeight(), w, h, image_w, image_h);
	ImGui::GetWindowDrawList()->AddImage(
		(void *)App->renderer3D->scene_texture_id,
		screen_pos,
		ImVec2(screen_pos.x + image_w, screen_pos.y + image_h),
		ImVec2(0, 1), 
		ImVec2(1, 0));

	if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->Rect(), (ImGuiID)"Scene"))
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
		{
			uint* id = (uint*)payload->Data;
			App->scene->CreateGameObject((ResourceModel*)App->resources->Get((*id)));
		}
		ImGui::EndDragDropTarget();
	}

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


void PanelScene::GetSizeWithAspectRatio(int current_width, int current_height, int wanted_width, int wanted_height, int& new_width, int& new_height)
{
	float scale_height = (float)wanted_height / (float)current_height;
	float scale_width = (float)wanted_width / (float)current_width;

	float scale = (std::min)(scale_height, scale_width);

	new_width = current_width * scale;
	new_height = current_height * scale;
}
