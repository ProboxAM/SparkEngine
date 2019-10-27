#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleCamera3D.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name = "Camera3D";

	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 3.0f, 5.0f);
	Reference = vec3(0.0f, 3.0f, 5.0f);

	Y = rotate(Y, -20, X);
	Z = rotate(Z, -20, X);
	Position = Reference + Z * length(Position);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

bool ModuleCamera3D::Init(const nlohmann::json::iterator& it)
{
	Load(it);

	return true;
}

bool ModuleCamera3D::Load(const nlohmann::json::iterator& it)
{
	movement_speed = (*it)["movement_speed"];
	focus_factor = (*it)["focus_factor"];
	camera_inputs_active = (*it)["inputs"];

	return true;
}

bool ModuleCamera3D::Save(nlohmann::json & it)
{
	it[name] = {
		{ "movement_speed",movement_speed },
		{ "focus_factor",focus_factor },
		{ "inputs", camera_inputs_active}
	};

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!
	newPos = { 0, 0, 0 };
	speed = movement_speed * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = movement_speed * 2 * dt;

	//if(App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (!isSelectedGOAsReference()) {
			SelectedGOAsReference();
			LookAt(Reference);
		}
		focusing = true;
		camera_inputs_active = false;		
	}

	if (focusing) Focus();


	CameraInputs();

	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		SelectedGOAsReference();//We set the select game object as reference so we now rotate around it.
		RotateAroundReference();
	}


	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		RotateAroundReference();
	}

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

bool ModuleCamera3D::isSelectedGOAsReference()
{
	float3 comparator = { Reference.x, Reference.y, Reference.z };
	if (App->scene->selected_gameobject->transform->position.Equals(comparator)) return true;
	else return false;
}

void ModuleCamera3D::CameraInputs()
{
	if (camera_inputs_active)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

		if (App->editor->IsInsideSceneWindow(math::float2(App->input->GetMouseX(), App->input->GetMouseY())))
		{
			if (App->input->GetMouseZ() > 0) newPos -= Z * speed;
			if (App->input->GetMouseZ() < 0) newPos += Z * speed;
		}
	}
}

void ModuleCamera3D::Focus()//If theres a selected game object the camera looks at the target and moves to it. It moves slower as the camera gets closer to the target
{
	SelectedGOAsReference();
	float3 end_position = { Reference.x, Reference.y, Reference.z };
	float3 position = { Position.x, Position.y, Position.z };
	float distance = position.Distance(end_position);
	speed = speed * distance * focus_factor;
	float bb_distance_aux = 0;
	if(App->scene->selected_gameobject->GetComponent(MESH))
		min_distance = App->scene->selected_gameobject->bounding_box.Size().Length()*focus_margin;
	else if (App->scene->selected_gameobject->transform->GetChildCount() > 0)
	{
		for (int i = 0; i < App->scene->selected_gameobject->transform->GetChildCount(); i++)
		{
			if (bb_distance_aux < App->scene->selected_gameobject->transform->GetChildren()[i]->gameobject->bounding_box.Size().Length()*focus_margin) {
				bb_distance_aux = App->scene->selected_gameobject->transform->GetChildren()[i]->gameobject->bounding_box.Size().Length()*focus_margin;
			}
		}
		min_distance = bb_distance_aux;
	}


	if (distance + threshold <= min_distance || distance - threshold >= min_distance) {
		if (distance < min_distance) {
			newPos += Z * speed;
			if (distance + newPos.z >= min_distance)
			{
				focusing = false;
				camera_inputs_active = true;
			}
		}
		else if (distance > min_distance) {
			newPos -= Z * speed;
			if (distance - newPos.z <= min_distance)
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

	float Sensitivity = 0.25f;

	Position -= Reference;

	if (dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;

		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);
		}
	}

	Position = Reference + Z * length(Position);
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

void ModuleCamera3D::SelectedGOAsReference()
{
	if (App->scene->selected_gameobject) {
		Reference = { App->scene->selected_gameobject->transform->position.x,  App->scene->selected_gameobject->transform->position.y, App->scene->selected_gameobject->transform->position.z };
	}
}
