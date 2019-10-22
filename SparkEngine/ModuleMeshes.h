#ifndef _MODULE_MESHES_H_
#define _MODULE_MESHES_H_

#include "Module.h"
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

class ModuleMeshes : public Module
{
public:
	ModuleMeshes(bool active);
	~ModuleMeshes();

	Mesh* LoadMesh(const aiScene* scene, const aiMesh* mesh) const;
	Mesh* CreatePrimitiveMesh(PRIMITIVE_TYPE type);
};


#endif // !_MODULE_MESHES_H_


