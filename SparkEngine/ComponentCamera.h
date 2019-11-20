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
	virtual ~ComponentCamera();

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

	void SetAsMainCamera(bool mode);

	Frustum frustum;

	bool Save(const nlohmann::json::iterator& it);
	bool Load(const nlohmann::json comp);

	bool active_camera = false;
	bool enable_frustum_culling = false;

private:
	void DrawFrustum();

	void Draw();
	
};

