#ifndef _MODULE_CAMERA_3D_H_
#define _MODULE_CAMERA_3D_H_

#include "Module.h"
#include "MathGeoLib/Geometry/LineSegment.h"

class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update();
	bool CleanUp();

	bool Init(const nlohmann::json::iterator& it);
	bool Load(const nlohmann::json::iterator& it);
	bool Save(nlohmann::json &it);

	void LookAt(const float3 &spot);
	LineSegment GetRaycast();

	bool focusing = false;
	float focus_distance = 5.f;
	float min_distance = 0.5f;
	float threshold = 0.5f;
	float focus_margin = 1.25f;

	bool camera_inputs_active = true;
	float movement_speed, focus_factor;
	float sensibility = 0.005f;

private:

	void SelectedGOAsReference();
	void HandleCameraInputs();
	void Focus();
	void RotateAroundReference();
	void HandleMouseClicking();



public:

	float3 reference, new_position;
	float speed;
	ComponentCamera* c_camera;

private:
	LineSegment picking;
	std::list<bool> hits;
};

#endif // !_MODULE_CAMERA_3D_H_