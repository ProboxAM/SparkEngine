#ifndef _MODULE_RENDERER_3D_H_
#define _MODULE_RENDERER_3D_H_

#include "Module.h"
#include "glmath.h"
#include "Light.h"
#include "Texture.h"
#define MAX_LIGHTS 8

typedef struct par_shapes_mesh_s par_shapes_mesh;
class Mesh;
class ComponentCamera;

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

	void DrawMesh(const Mesh* m, const Texture* tex, const float4x4& mtransform) const;
	void DebugVertexNormals(const Mesh* m, const float4x4& mtransform) const;
	void DebugFaceNormals(const Mesh* m, const float4x4& mtransform) const;
	void DebugDrawCube(const float3 * vertices, Color color) const;
	void DebugDrawLines(std::vector<float3> lines);

	void ResizeScene(float w, float h);
	void UpdateProjectionMatrix();

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
	float3 bkg_color = float3(0.1f, 0.1f, 0.1f);

	ComponentCamera* c_camera;

private:
	bool wireframe;
	bool vsync;
	bool depth_test, cull_face, lighting, color_material, texture2d;
	uint scene_depth_id;

};

#endif // !_MODULE_RENDERER_3D_H_