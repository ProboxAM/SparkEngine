#pragma once
#include "Module.h"
#include "glmath.h"
#include "Light.h"
#include "Texture.h"
#define MAX_LIGHTS 8

typedef struct par_shapes_mesh_s par_shapes_mesh;
class Mesh;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(nlohmann::json::iterator it);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	bool Save(nlohmann::json &it);
	void GLEnable(unsigned int flag, bool active);
	void SetWireframeMode(bool on);


	void DrawMesh(Mesh* m, Texture* tex);
	void DebugVertexNormals(Mesh* m);
	void DebugFaceNormals(Mesh* m);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

private:
	bool vsync;

	//TESTING STUFF
	uint my_vertex = 0;
	uint my_indices = 0;
	par_shapes_mesh* my_sphere = nullptr;
	
public:
	std::vector<Mesh> test_meshes;
	std::vector<Mesh> test_meshes_dropped;

	Texture test_texture;
	Texture test_texture_lena;
};