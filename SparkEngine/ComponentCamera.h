#pragma once
#include "Component.h"

#include "MathGeoLib/MathBuildConfig.h"
#include "MathGeoLib/Geometry/GeometryAll.h"
#include "MathGeoLib/Math/MathAll.h"

class ComponentCamera :
	public Component
{
public:
	ComponentCamera(GameObject* gameobject);
	~ComponentCamera();

	void SetFrustumNearPlaneDistance(float distance);
	void SetFrustumFarPlaneDistance(float distance);
	void SetFrustumFOV(float fov, bool degrees = false);
	void SetFrustumAspectRatio(float aspect_ratio);

	bool Intersects(AABB box);

	Plane GetFrustumNearPlane();
	float GetFrustumNearPlaneDistance();
	Plane GetFrustumFarPlane();
	float GetFrustumFarPlaneDistance();
	float GetFrustumHorizontalFOV(bool degrees = false);
	float GetFrustumVerticalFOV(bool degrees = false);
	float GetFrustumAspectRatio();

	void LookAt(float3 position);

	float4x4 GetOpenGLViewMatrix();
	float4x4 GetOpenGLProjectionMatrix();

	void UpdateFrustumTransform();

	Frustum frustum;

	bool update_camera_projection = false;

private:

	bool active_camera = false;

	void DrawFrustum();

	void Update(float dt);
	
};

