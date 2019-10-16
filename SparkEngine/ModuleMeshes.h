#pragma once
#include "Module.h"
class Mesh;
struct aiScene;
struct aiMesh;

class ModuleMeshes : public Module
{
public:
	ModuleMeshes(bool active);
	~ModuleMeshes();

	Mesh* LoadMesh(const aiScene* scene, aiMesh* mesh);
	Mesh* CreateSphereMesh();
	Mesh* CreateSquareMesh();
};

