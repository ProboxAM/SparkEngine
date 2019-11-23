#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleTime.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "PanelScene.h"
#include "ModuleCamera3D.h"

#include "glew/glew.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name = "Camera3D";
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	App->renderer3D->editor_camera = nullptr;

	return true;
}

bool ModuleCamera3D::Init(const nlohmann::json::iterator& it)
{
	LOG("Setting up the camera");
	bool ret = true;

	c_camera = new ComponentCamera(nullptr);

	c_camera->frustum.pos = { 20.f, 50.f, 0.f };
	c_camera->SetFrustumFarPlaneDistance(1000.0f);
	LookAt({ 0.f, 0.f, 0.f });

	App->renderer3D->editor_camera = c_camera;

	Load(it);

	return true;
}

bool ModuleCamera3D::Load(const nlohmann::json::iterator& it)
{
	movement_speed = (*it)["movement_speed"];
	focus_factor = (*it)["focus_factor"];
	camera_inputs_active = (*it)["inputs"];
	c_camera->SetFrustumFOV((*it)["fov"]);

	return true;
}

bool ModuleCamera3D::Save(nlohmann::json & it)
{
	it[name] = {
		{ "movement_speed",movement_speed },
		{ "focus_factor",focus_factor },
		{ "inputs", camera_inputs_active},
		{ "fov", c_camera->frustum.verticalFov}
	};



	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update()
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !ImGuizmo::IsOver())
		HandleMouseClicking();

	new_position = { 0, 0, 0 };
	speed = movement_speed * App->time->RealTimeDeltaTime();
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = movement_speed * 2 * App->time->RealTimeDeltaTime();

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (App->scene->selected_gameobject)
		{
			SelectedGOAsReference();
			LookAt(reference);
			focusing = true;
			camera_inputs_active = false;

			if (App->scene->selected_gameobject->GetComponent(MESH))
				min_distance = App->scene->selected_gameobject->GetDistanceFromAABB() * focus_margin;
			else min_distance = focus_distance;

		}
	}

	if (focusing) Focus();

	HandleCameraInputs();

	c_camera->frustum.pos += new_position;
	reference += new_position;

	// Mouse motion ----------------
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT){
		SelectedGOAsReference();//We set the select game object as reference so we now rotate around it.
		RotateAroundReference();
	}


	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		RotateAroundReference();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3 &spot)
{
	c_camera->LookAt(spot);
	reference = spot;
}

void ModuleCamera3D::HandleCameraInputs()
{
	if (camera_inputs_active)
	{
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) new_position += c_camera->frustum.front * speed;
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) new_position -= c_camera->frustum.front * speed;


			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) new_position -= c_camera->frustum.WorldRight() * speed;
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) new_position += c_camera->frustum.WorldRight() * speed;
		}

		if (App->editor->IsInsideSceneWindow(math::float2(App->input->GetMouseX(), App->input->GetMouseY())))
		{
			if (App->input->GetMouseZ() > 0) new_position += c_camera->frustum.front * speed;
			if (App->input->GetMouseZ() < 0) new_position -= c_camera->frustum.front * speed;
		}
	}
}

void ModuleCamera3D::Focus()//If theres a selected game object the camera looks at the target and moves to it. It moves slower as the camera gets closer to the target
{
	SelectedGOAsReference();
	float3 end_position = { reference.x, reference.y, reference.z };
	float3 position = { c_camera->frustum.pos.x, c_camera->frustum.pos.y, c_camera->frustum.pos.z };
	float distance = position.Distance(end_position);
	speed = speed * distance * focus_factor;

	if (distance + threshold*distance/2 <= min_distance || distance - threshold*distance/2 >= min_distance) {
		if (distance < min_distance) {
			new_position -= c_camera->frustum.front * speed;
			if (distance + new_position.z >= min_distance)
			{
				focusing = false;
				camera_inputs_active = true;
			}
		}
		else if (distance > min_distance) {
			new_position += c_camera->frustum.front * speed;
			if (distance - new_position.z <= min_distance)
			{
				focusing = false;
				camera_inputs_active = true;
			}
		}
	}
	else {
		focusing = false;
		camera_inputs_active = true;
	}
	
}

void ModuleCamera3D::RotateAroundReference()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	float3 direction = c_camera->frustum.pos - reference;

	Quat quat_y(c_camera->frustum.up, dx*sensibility);
	Quat quat_x(c_camera->frustum.WorldRight(), dy*sensibility);

	direction = quat_x.Transform(direction);
	direction = quat_y.Transform(direction);

	c_camera->frustum.pos = direction + reference;
	LookAt(reference);
}

void ModuleCamera3D::HandleMouseClicking()
{
	if (App->editor->IsInsideSceneWindow({ (float)App->input->GetMouseX(), (float)App->input->GetMouseY() })) {

		float2 mouse_position, normalized_mouse_position, screen_position;

		PanelScene* ps = (PanelScene*)App->editor->GetPanels()[SCENE];
		ps->GetScreenPos(screen_position.x, screen_position.y);

		mouse_position = { ((float)App->input->GetMouseX() - (screen_position.x + (ps->GetScreenWidth() / 2))),
			((float)App->input->GetMouseY() - (screen_position.y + (ps->image_h / 2))) };

		normalized_mouse_position = { mouse_position.x / ps->image_w * 2, mouse_position.y / ps->image_h * 2 };

		LOG("x: %f, y: %f", normalized_mouse_position.x, normalized_mouse_position.y);

		picking = c_camera->frustum.UnProjectLineSegment(normalized_mouse_position.x, -normalized_mouse_position.y);

		App->scene->OnMousePicking(picking);
	}
}

void ModuleCamera3D::SelectedGOAsReference()
{
	if (App->scene->selected_gameobject) {
		reference = { App->scene->selected_gameobject->transform->GetPosition().x,  App->scene->selected_gameobject->transform->GetPosition().y, App->scene->selected_gameobject->transform->GetPosition().z };
	}
}
