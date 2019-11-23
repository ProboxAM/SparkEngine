#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentTexture.h"
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

	w = ImGui::GetWindowWidth();
	h = ImGui::GetWindowHeight() - CHECKBOX_SIZE;
	GetSizeWithAspectRatio(App->window->GetWindowWidth(), App->window->GetWindowHeight(), w, h, image_w, image_h);
	screen_pos = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + (h * 0.5f - image_h * 0.5f));
	ImGui::GetWindowDrawList()->AddImage(
		(void *)App->renderer3D->scene_texture_id,
		screen_pos,
		ImVec2(screen_pos.x + image_w, screen_pos.y + image_h),
		ImVec2(0, 1),
		ImVec2(1, 0));

	if (App->scene->selected_gameobject) {
		DrawTransformGuizmo();
	}

	if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->Rect(), (ImGuiID)"Scene"))
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET"))
		{
			uint* id = (uint*)payload->Data;
			Resource* res = App->resources->Get((*id));
			if (res)
			{
				if (res->GetType() == Resource::RESOURCE_TYPE::R_MODEL)
				{
					res->AddReference();
					App->scene->CreateGameObject((ResourceModel*)res);
					res->RemoveReference();
				}
				else if (res->GetType() == Resource::RESOURCE_TYPE::R_TEXTURE)
				{
					GameObject* selected_object = App->scene->OnMousePicking(App->camera->GetRaycast());
					if (selected_object)
					{
						ComponentTexture* c_texture = (ComponentTexture*)selected_object->GetComponent(COMPONENT_TYPE::TEXTURE);
						if (c_texture)
						{
							res->AddReference();
							c_texture->AddTexture((ResourceTexture*)res);
						}
					}
				}
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
		if (ImGui::Button("Camera"))
			ImGui::OpenPopup("Camera popup");
		if (ImGui::BeginPopup("Camera popup"))
		{
			float fov = App->camera->c_camera->GetFrustumVerticalFOV(true);
			if (ImGui::SliderFloat("Field of View", &fov, 10, 120, "%.0f"))
				App->camera->c_camera->SetFrustumFOV(fov, true);
			ImGui::EndPopup();
		}
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


void PanelScene::DrawTransformGuizmo()
{
	//ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
	float4x4 delta, transform, view, projection;

	transform = App->scene->selected_gameobject->transform->GetTransformMatrix().Transposed();
	projection = App->renderer3D->editor_camera->GetOpenGLProjectionMatrix();
	view = App->renderer3D->editor_camera->GetOpenGLViewMatrix();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(float(screen_pos.x), float(screen_pos.y), float(image_w), float(image_h));
	ImGuizmo::SetDrawlist();
	ImGuizmo::Manipulate((const float*)&view,
		(const float*)&projection,
		App->editor->guizmo_operation,
		App->editor->guizmo_mode, 
		(float*)&transform,
		(float*)&delta
	);

	if (ImGuizmo::IsUsing())
	{
		if(!App->scene->selected_gameobject->isStatic())
			App->scene->selected_gameobject->transform->SetTransformMatrix(transform.Transposed());
	}
}
