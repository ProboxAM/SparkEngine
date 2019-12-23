#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ComponentTexture.h"
#include "GameObject.h"
#include "Quadtree.h"
#include "ResourceMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "ComponentMesh.h"

#include "glew\glew.h"



ComponentMesh::ComponentMesh(GameObject* gameobject) : Component(gameobject)
{
}


ComponentMesh::~ComponentMesh()
{
	mesh->RemoveReference();
	mesh = nullptr;
}

void ComponentMesh::Update()
{

}

void ComponentMesh::Draw()
{
	ComponentTexture* c_tex = (ComponentTexture*)gameobject->GetComponent(COMPONENT_TYPE::TEXTURE);
	float4x4 transform = gameobject->transform->GetTransformMatrix();
	if (mesh && to_draw) {
		App->renderer3D->DrawMesh(mesh, c_tex->active ? c_tex->GetTexture() : nullptr, transform);
		if (App->scene->selected_gameobject == gameobject)
			if (App->renderer3D->debug_draw)
				App->renderer3D->DrawOutline(mesh, { 0.9f, 1.f, 0.1f }, transform);
	}

	if (App->renderer3D->debug_draw)
	{
		if (debug_vertex_normal)
			App->renderer3D->DebugVertexNormals(mesh, transform);
		if (debug_face_normal)
			App->renderer3D->DebugFaceNormals(mesh, transform);
		if (debug_bounding_box) {
			static float3 corners[8];
			gameobject->aabb.GetCornerPoints(corners);
			App->renderer3D->DebugDrawCube(corners, { 255, 0, 0, 255 });
			gameobject->obb.GetCornerPoints(corners);
			App->renderer3D->DebugDrawCube(corners, { 0, 0, 255, 255 });
		}
	}

	to_draw = false;
}

void ComponentMesh::AddMesh(ResourceMesh * mesh)
{
	this->mesh = mesh;
	gameobject->aabb = GetAABB();
	gameobject->obb = GetAABB();
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
		{"type", type},
		{"resource", mesh->GetID()},
		{"debug_bb", debug_bounding_box},
		{"debug_face_n", debug_face_normal },
		{"debug_vertex_n", debug_vertex_normal }
	};

	it.value().push_back(object);

	return true;
}

bool ComponentMesh::Load(const nlohmann::json comp)
{
	active = comp["active"];
	type = comp["type"];
	AddMesh((ResourceMesh*)App->resources->GetAndReference(comp["resource"]));
	debug_bounding_box = comp["debug_bb"];
	debug_face_normal = comp["debug_face_n"];
	debug_vertex_normal = comp["debug_vertex_n"];

	return true;
}
ResourceMesh * ComponentMesh::GetMesh()
{
	return mesh;
}

void ComponentMesh::AttachSkeleton(ComponentTransform * root)
{
	root_bone = root;

	//Duplicate mesh
	deformable_mesh = new ResourceMesh(App->GenerateID(), mesh);
}
