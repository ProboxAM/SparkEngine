#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "Texture.h"
#include "ComponentMesh.h"



ComponentMesh::ComponentMesh() : Component(gameobject)
{
}


ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Update()
{
	Texture* tex = my_go->
	if (mesh)
		App->renderer3D->DrawMesh(mesh);

	if (debug_vertex_normal)
		App->renderer3D->DebugVertexNormals(mesh);
	if (debug_face_normal)
		App->renderer3D->DebugFaceNormals(mesh);
}
