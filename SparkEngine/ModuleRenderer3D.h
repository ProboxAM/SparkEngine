#ifndef _MODULE_RENDERER_3D_H_
#define _MODULE_RENDERER_3D_H_

#include "Module.h"
#include "glmath.h"
#include "Light.h"
#include "ResourceTexture.h"
#define MAX_LIGHTS 8

typedef struct par_shapes_mesh_s par_shapes_mesh;
class ResourceMesh;
class ComponentCamera;
class ComponentMesh;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(const nlohmann::json::iterator& it);
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	void OnResize(int width, int height);
	bool Save(nlohmann::json &it);
	bool Load(const nlohmann::json::iterator& it);
	void GLEnable(unsigned int flag, bool active);
	bool IsEnabled(unsigned int flag) const;

	void SetWireframeMode(bool on);

	void DrawMesh(const ResourceMesh* m, const ResourceTexture* tex, const float4x4& mtransform, const ResourceMesh* deform_m = nullptr) const;
	void DebugVertexNormals(const ResourceMesh* m, const float4x4& mtransform) const;
	void DebugFaceNormals(const ResourceMesh* m, const float4x4& mtransform) const;
	void DebugDrawCube(const float3 * vertices, Color color) const;
	void DebugDrawLines(std::vector<float3> lines);
	void DrawSphere(float3 position, float radius);
	void DrawSphere(float4x4 matrix, float radius);

	void DrawOutline(const ResourceMesh* m, const Color color, const float4x4& mtransform) const;

	bool IsWireframeEnabled() const;
	bool GetVsync() const;
	void SetVsync(bool active);

private:
	void DrawSceneViewPort();
	void DrawGameViewPort();

	void CreateSceneBuffer();
	void CreateGameBuffer();

	void ResizeScene(float w, float h);
	void ResizeGame(float w, float h);

	void UpdateSceneProjectionMatrix();
	void UpdateGameProjectionMatrix();

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;

	///Buffer and texture for rendering into Scene window
	uint scene_buffer_id = -1;
	uint scene_texture_id;

	uint game_buffer_id = -1;
	uint game_texture_id;

	float3 bkg_color = float3(0.1f, 0.1f, 0.1f);

	ComponentCamera* editor_camera = nullptr;
	ComponentCamera* game_camera = nullptr;

	bool debug_draw = false;

private:
	bool wireframe;
	bool vsync;
	bool depth_test, cull_face, lighting, color_material, texture2d;
	uint scene_depth_id, game_depth_id;
	std::vector<ComponentMesh*> meshes;

};

#endif // !_MODULE_RENDERER_3D_H_