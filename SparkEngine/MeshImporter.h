#ifndef _MESH_IMPORTER_H_
#define _MESH_IMPORTER_H_

#include "Importer.h"

class Mesh;
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
	bool CleanUp();

	Mesh* Import(const aiScene* scene, const aiMesh* mesh);

	Mesh* Load(const char* exported_file);
	Mesh* LoadPrimitive(PRIMITIVE_TYPE type);
};

#endif // !_MESH_IMPORTER_H_

