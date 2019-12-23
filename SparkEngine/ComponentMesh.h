#ifndef _COMPONENT_MESH_H_
#define _COMPONENT_MESH_H_

#include "Component.h"

class ResourceMesh;
class ComponentBone;
class ComponentTransform;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* gameobject);
	virtual ~ComponentMesh();

	void Update();
	void Draw();
	void AddMesh(ResourceMesh* tex);

	void SetDebugVertexNormal();
	void SetDebugFaceNormal();

	void SetDebugBoundingBox();

	int GetVerticesAmount();
	int GetNormalsAmount();
	int GetUVAmount();
	int GetIndicesAmount();

	AABB GetAABB();

	bool Save(const nlohmann::json::iterator& it);
	bool Load(const nlohmann::json comp);

	ResourceMesh* GetMesh();
	void AttachSkeleton(ComponentTransform* root);

private:
	void AttachBone(ComponentTransform * bone_transform);
	void UpdateDeformableMesh();
	void ResetDeformableMesh();

private:
	ResourceMesh* mesh = nullptr;
	ResourceMesh* deformable_mesh = nullptr;
	std::vector<ComponentBone*> bones;

public:
	bool debug_vertex_normal = false, debug_face_normal = false, debug_bounding_box = false;
	bool to_draw = false;

	ComponentTransform* root_bone = nullptr;
};

#endif // !_COMPONENT_MESH_H_

