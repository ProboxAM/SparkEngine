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

	bool Init(const nlohmann::json::iterator& it);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);
	bool Save(nlohmann::json &it);
	bool Load(const nlohmann::json::iterator& it);
	void GLEnable(unsigned int flag, bool active);
	bool IsEnabled(unsigned int flag) const;

	void SetWireframeMode(bool on);

	void DrawMesh(const Mesh* m, const Texture* tex) const;
	void DebugVertexNormals(const Mesh* m) const;
	void DebugFaceNormals(const Mesh* m) const;

	void ResizeScene(float w, float h);

	bool IsWireframeEnabled() const;
	bool GetVsync() const;
	void SetVsync(bool active);

private:
	void CreateSceneBuffer();

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	///Buffer and texture for rendering into Scene window
	uint scene_buffer_id = -1;
	uint scene_texture_id;

private:
	bool wireframe;
	bool vsync;
	bool depth_test, cull_face, lighting, color_material, texture2d;
	uint scene_depth_id;

};