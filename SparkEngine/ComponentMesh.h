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
private:
	Mesh* mesh;
	bool debug_vertex_normal, debug_face_normal;
};

