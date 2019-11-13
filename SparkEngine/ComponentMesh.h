#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include "Component.h"

class Mesh;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* gameobject);
	~ComponentMesh();

	void Update(float dt);
	void AddMesh(Mesh* tex);

	void SetDebugVertexNormal();
	void SetDebugFaceNormal();

	void SetDebugBoundingBox();

	int GetVerticesAmount();
	int GetNormalsAmount();
	int GetUVAmount();
	int GetIndicesAmount();

	AABB GetAABB();

	bool Save(const nlohmann::json::iterator& it);

	Mesh* GetMesh();

private:
	Mesh* mesh = nullptr;

public:
	bool debug_vertex_normal = false, debug_face_normal = false, debug_bounding_box = false;
};

#endif // !_COMPONENT_MESH_H_

