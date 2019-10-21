#pragma once
#include "Module.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Init(nlohmann::json::iterator it);
	bool Load(nlohmann::json::iterator it);
	bool Save(nlohmann::json &it);

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

	bool focusing = false;
	float focus_distance = 5.f;
	float min_distance = 15.f;

	bool camera_inputs_active = true;
	float movement_speed, sprint_speed, focus_factor;
private:

	void CalculateViewMatrix();
	bool GOSelectedAsReference();
	void CameraInputs();
	void Focus();

public:
	
	vec3 X, Y, Z, Position, Reference, newPos;
	float speed;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};