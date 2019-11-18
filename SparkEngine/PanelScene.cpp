#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Geometry\AABB2D.h"
#include "ImGui/imgui.h"
#include "glew/glew.h"

#include "PanelScene.h"

#define CHECKBOX_SIZE 50

PanelScene::PanelScene(bool active): Panel(active)
{
}


PanelScene::~PanelScene()
{

}

void PanelScene::Draw()
{
	ImGui::Begin("Scene", &active, ImGuiWindowFlags_MenuBar);
	ImGui::ShowDemoWindow();

	w = ImGui::GetWindowWidth();
	h = ImGui::GetWindowHeight() - CHECKBOX_SIZE;
	screen_pos = ImGui::GetCursorScreenPos();
	GetSizeWithAspectRatio(App->window->GetWindowWidth(), App->window->GetWindowHeight(), w, h, image_w, image_h);
	ImGui::GetWindowDrawList()->AddImage(
		(void *)App->renderer3D->scene_texture_id,
		screen_pos,
		ImVec2(screen_pos.x + image_w, screen_pos.y + image_h),
		ImVec2(0, 1),
		ImVec2(1, 0));

	if (App->scene->selected_gameobject) {
		HandleTransformInputs();
		DrawTransformGuizmo();
	}

	if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->Rect(), (ImGuiID)"Scene"))
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
		{
			uint* id = (uint*)payload->Data;
			Resource* res = App->resources->GetAndLoad((*id));
			if (res)
			{
				if(res->GetType() == Resource::RESOURCE_TYPE::R_MODEL)
					App->scene->CreateGameObject((ResourceModel*) res);
			}		
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
		if (ImGui::Button(mode.c_str())) {
			SetGlobalMode(!App->scene->global_mode);
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void PanelScene::Start()
{
	guizmo_mode = ImGuizmo::MODE::LOCAL;
	guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
	mode = "Local";
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

void PanelScene::HandleTransformInputs()
{
	if (!ImGuizmo::IsUsing())
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			guizmo_operation = ImGuizmo::OPERATION::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			guizmo_operation = ImGuizmo::OPERATION::SCALE;
	}
}

void PanelScene::DrawTransformGuizmo()
{
	//ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
	float4x4 delta, transform, view, projection;

	transform = App->scene->selected_gameobject->transform->GetTransformMatrix().Transposed();
	projection = App->renderer3D->c_camera->GetOpenGLProjectionMatrix();
	view = App->renderer3D->c_camera->GetOpenGLViewMatrix();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(float(ImGui::GetCursorScreenPos().x), float(ImGui::GetCursorScreenPos().y), float(w), float(h));
	ImGuizmo::SetDrawlist();
	ImGuizmo::Manipulate((const float*)&view,
		(const float*)&projection,
		guizmo_operation, 
		guizmo_mode, 
		(float*)&transform,
		(float*)&delta
	);

	if (ImGuizmo::IsUsing())
	{
		App->scene->selected_gameobject->transform->SetTransformMatrix(transform.Transposed());
	}
}

void PanelScene::SetGlobalMode(bool on)
{
	if (on) {
		mode = "World";
		guizmo_mode = ImGuizmo::MODE::WORLD;
		App->scene->global_mode = true;
	}
	else {
		mode = "Local";
		guizmo_mode = ImGuizmo::MODE::LOCAL;
		App->scene->global_mode = false;
	}
}
