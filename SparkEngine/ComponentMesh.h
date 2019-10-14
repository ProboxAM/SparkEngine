#pragma once
#include "Component.h"

class Mesh;

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	~ComponentMesh();

	void Update();

private:
	Mesh* mesh;
	bool debug_vertex_normal, debug_face_normal;
};

