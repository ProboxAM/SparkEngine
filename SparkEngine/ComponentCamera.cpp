#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"




ComponentCamera::ComponentCamera(GameObject* gameobject) : Component(gameobject)
{
	frustum.pos = { 0.f, 0.f, 0.f };
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = .1f;
	frustum.farPlaneDistance = 500.0f;

	SetFrustumFOV(60, true);
}


ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::SetFrustumNearPlaneDistance(float distance)
{
	frustum.nearPlaneDistance = distance;
}

void ComponentCamera::SetFrustumFarPlaneDistance(float distance)
{
	frustum.farPlaneDistance = distance;
}

void ComponentCamera::SetFrustumFOV(float fov, bool degrees)
{
	if(degrees)
		frustum.verticalFov = DegToRad(fov);
	else frustum.verticalFov = fov;

	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * App->window->GetWindowWidth() / App->window->GetWindowHeight());

	update_camera_projection = true;
}

bool ComponentCamera::Intersects(AABB box)
{
	return frustum.Intersects(box);
}

Plane ComponentCamera::GetFrustumNearPlane()
{
	return frustum.NearPlane();
}

float ComponentCamera::GetFrustumNearPlaneDistance()
{
	return frustum.nearPlaneDistance;
}

Plane ComponentCamera::GetFrustumFarPlane()
{
	return frustum.FarPlane();
}

float ComponentCamera::GetFrustumFarPlaneDistance()
{
	return frustum.farPlaneDistance;
}

float ComponentCamera::GetFrustumHorizontalFOV(bool degrees)
{
	if (degrees)
		return (RadToDeg(frustum.horizontalFov));
	else
		return frustum.horizontalFov;
}

float ComponentCamera::GetFrustumVerticalFOV(bool degrees)
{
	if (degrees)
		return RadToDeg(frustum.verticalFov);
	else
		return frustum.verticalFov;
}

float ComponentCamera::GetFrustumAspectRatio()
{
	return frustum.AspectRatio();
}

void ComponentCamera::LookAt(float3 position)
{
	float3 direction = position - frustum.pos;

	float3x3 matrix = float3x3::LookAt(frustum.front, direction.Normalized(), frustum.up, float3::unitY);

	frustum.front = (matrix.MulDir(frustum.front).Normalized());
	frustum.up = (matrix.MulDir(frustum.up).Normalized());
}

float4x4 ComponentCamera::GetOpenGLViewMatrix()
{
	float4x4 matrix = frustum.ViewMatrix();
	return matrix.Transposed();
}

float4x4 ComponentCamera::GetOpenGLProjectionMatrix()
{
	return frustum.ProjectionMatrix().Transposed();
}

void ComponentCamera::DrawFrustum()
{
	float3 corners[8];
	frustum.GetCornerPoints(corners);
	App->renderer3D->DebugDrawCube(corners, {1.f, 1.f, 1.f, 1.f});
}

void ComponentCamera::UpdateFrustumTransform()
{
	float4x4 transform_matrix;
	if(gameobject) transform_matrix = gameobject->transform->GetTransformMatrix();
	else transform_matrix = float4x4::identity;
	frustum.pos = transform_matrix.TranslatePart();
	frustum.up = transform_matrix.WorldY();
	frustum.front = transform_matrix.WorldZ();
}

bool ComponentCamera::Save(const nlohmann::json::iterator & it)
{
	nlohmann::json object = {
	{ "active", active },
	{ "type", type },
	{ "active_camera", active_camera },
	{ "fov", frustum.verticalFov },
	{ "near_distance", frustum.nearPlaneDistance },
	{ "far_distance", frustum.farPlaneDistance }
	};

	it.value().push_back(object);

	return true;
}

bool ComponentCamera::Load(const nlohmann::json comp)
{
	active = comp["active"];
	type = comp["type"];
	active_camera = comp["active_camera"];

	SetFrustumNearPlaneDistance(comp["near_distance"]);
	SetFrustumFarPlaneDistance(comp["far_distance"]);
	SetFrustumFOV(comp["fov"]);

	UpdateFrustumTransform();

	return true;
}

void ComponentCamera::Draw()
{
	DrawFrustum();
}

