#ifndef _MESH_IMPORTER_H_
#define _MESH_IMPORTER_H_

#include "Importer.h"

class ResourceMesh;
struct aiScene;
struct aiMesh;

enum PRIMITIVE_TYPE {
	P_CUBE,
	P_SPHERE,
	P_CYLINDER,
	P_PLANE,
	P_CONE
};

class MeshImporter : public Importer
{
public:
	MeshImporter();
	~MeshImporter();

	bool Init();
	bool Start();
	bool CleanUp();

	uint Import(const aiScene* scene, const aiMesh* mesh, uint id);
	bool SaveMesh(ResourceMesh* mesh);

	bool Load(ResourceMesh* resources);
	ResourceMesh* LoadPrimitive(PRIMITIVE_TYPE type, ResourceMesh* mesh);

public:
	uint cube, sphere, cylinder, cone, plane;
};

#endif // !_MESH_IMPORTER_H_

