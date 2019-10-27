#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

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

	bool Init(const nlohmann::json::iterator& it);
	bool Load(const nlohmann::json::iterator& it);
	bool Save(nlohmann::json &it);

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

	bool focusing = false;
	float focus_distance = 5.f;
	float min_distance = 0.5f;
	float threshold = 0.5f;
	float focus_margin = 1.25f;

	bool camera_inputs_active = true;
	float movement_speed, focus_factor;
private:

	void CalculateViewMatrix();
	void SelectedGOAsReference();
	bool isSelectedGOAsReference();
	void CameraInputs();
	void Focus();
	void RotateAroundReference();

public:

	vec3 X, Y, Z, Position, Reference, newPos;
	float speed;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};

#endif // !_MODULE_CAMERA_3D_H_