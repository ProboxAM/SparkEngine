#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ComponentTexture.h"
#include "GameObject.h"
#include "Texture.h"
#include "ComponentMesh.h"



ComponentMesh::ComponentMesh(GameObject* gameobject) : Component(gameobject)
{
}


ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Update(float dt)
{
	Texture* tex = ((ComponentTexture*)gameobject->GetComponent(COMPONENT_TYPE::TEXTURE))->GetTexture();
	if (mesh && tex)
		App->renderer3D->DrawMesh(mesh, tex);

	if (debug_vertex_normal)
		App->renderer3D->DebugVertexNormals(mesh);
	if (debug_face_normal)
		App->renderer3D->DebugFaceNormals(mesh);
}

void ComponentMesh::AddMesh(Mesh * mesh)
{
	this->mesh = mesh;
}

void ComponentMesh::SetDebugVertexNormal()
{
	debug_vertex_normal = !debug_vertex_normal;
}

void ComponentMesh::SetDebugFaceNormal()
{
	debug_face_normal = !debug_face_normal;
}
