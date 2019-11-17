#pragma once

#include "GameObject.h"
#include "MathGeoLib/Geometry/AABB.h"

#define MAX_BUCKET_SIZE 2
#define CHILDREN_SIZE 4

class QuadtreeNode
{
public:
	QuadtreeNode();
	~QuadtreeNode();

	void Create(const AABB limits);
	void Clear();
	void InsertGameObject(const GameObject* gameobject);
	void RemoveGameObject(const GameObject* gameobject);

	template<typename PRIMITIVE>
	void Intersect(std::vector<GameObject*>&, const PRIMITIVE & primitive);

private:
	void Split();
	void DistributeChildren();

	QuadtreeNode* children[4];
	QuadtreeNode* parent = nullptr;

	std::vector<const GameObject*> bucket;

public:
	AABB box;
	void Draw();
};

class Quadtree
{
public:
	Quadtree();
	~Quadtree();

	void Create(const AABB limits);
	void Clear();
	void InsertGameObject(const GameObject* gameobject);
	void RemoveGameObject(const GameObject* gameobject);

	template<typename PRIMITIVE>
	void Intersect(std::vector<GameObject*>&, const PRIMITIVE & primitive);

	void Draw();
private:
	QuadtreeNode* root;
};

template<typename PRIMITIVE>
inline void Quadtree::Intersect(std::vector<GameObject*>&, const PRIMITIVE & primitive)
{
}

template<typename PRIMITIVE>
inline void QuadtreeNode::Intersect(std::vector<GameObject*>&, const PRIMITIVE & primitive)
{
}
