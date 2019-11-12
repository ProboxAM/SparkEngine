#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ComponentTexture.h"
#include "GameObject.h"
#include "ResourceMesh.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "glew\glew.h"



ComponentMesh::ComponentMesh(GameObject* gameobject) : Component(gameobject)
{
}


ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Update(float dt)
{
	ComponentTexture* c_tex = (ComponentTexture*)gameobject->GetComponent(COMPONENT_TYPE::TEXTURE);
	float4x4 transform = gameobject->transform->GetTransformMatrix();
	if (mesh && App->renderer3D->c_camera->frustum.ContainsOrIntersectsAABB(gameobject->global_aabb))
		App->renderer3D->DrawMesh(mesh, c_tex->active ? c_tex->GetTexture() : nullptr , transform);

	if (debug_vertex_normal)
		App->renderer3D->DebugVertexNormals(mesh, transform);
	if (debug_face_normal)
		App->renderer3D->DebugFaceNormals(mesh, transform);
	if (debug_bounding_box) {
		static float3 corners[8];
		gameobject->global_aabb.GetCornerPoints(corners);
		App->renderer3D->DebugDrawCube(corners, {255, 0, 0, 255});
		gameobject->global_obb.GetCornerPoints(corners);
		App->renderer3D->DebugDrawCube(corners, { 0, 0, 255, 255 });
	}
}

void ComponentMesh::AddMesh(ResourceMesh * mesh)
{
	this->mesh = mesh;
	gameobject->global_aabb = GetAABB();
	gameobject->global_obb = GetAABB();
	gameobject->UpdateBBox();
}

void ComponentMesh::SetDebugVertexNormal()
{
	debug_vertex_normal = !debug_vertex_normal;
}

void ComponentMesh::SetDebugFaceNormal()
{
	debug_face_normal = !debug_face_normal;
}

void ComponentMesh::SetDebugBoundingBox()
{
	debug_bounding_box = !debug_bounding_box;
}

int ComponentMesh::GetVerticesAmount()
{
	return mesh->total_vertices;
}

int ComponentMesh::GetNormalsAmount()
{
	return mesh->total_normal;
}

int ComponentMesh::GetUVAmount()
{
	return mesh->total_uv;
}

int ComponentMesh::GetIndicesAmount()
{
	return mesh->total_indices;
}

AABB ComponentMesh::GetAABB()
{
	AABB bounding_box;
	bounding_box.SetNegativeInfinity();
	bounding_box.Enclose(mesh->vertices, mesh->total_vertices);
	return bounding_box;
}

bool ComponentMesh::Save(const nlohmann::json::iterator & it)
{
	nlohmann::json object = {
		{"active", active },
		{"type",type},
		{"debug_bb", debug_bounding_box},
		{"debug_face_n", debug_face_normal },
		{"debug_vertex_n", debug_vertex_normal }
	};

	it.value().push_back(object);

	return true;
}
