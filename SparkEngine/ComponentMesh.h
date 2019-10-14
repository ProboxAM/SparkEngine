#pragma once
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

	int GetVerticesAmount();
	int GetNormalsAmount();
	int GetUVAmount();
	int GetIndicesAmount();

private:
	Mesh* mesh;
	bool debug_vertex_normal, debug_face_normal;
};

