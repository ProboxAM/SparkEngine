#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#define MAX_BUCKET_SIZE 2
#define CHILDREN_SIZE 4

class GameObject;
#include "MathGeoLib/Geometry/AABB.h"

class QuadtreeNode
{
public:
	QuadtreeNode();
	~QuadtreeNode();

	void Create(const AABB limits);
	void Clear();
	void InsertGameObject(GameObject* gameobject);
	void RemoveGameObject(GameObject* gameobject);

	template<typename PRIMITIVE>
	void CollectIntersections(std::vector<GameObject*>&, const PRIMITIVE & primitive);
	template<typename PRIMITIVE>
	void CollectIntersections(std::map<float, GameObject*>&, const PRIMITIVE & primitive);
private:
	void Split();
	void DistributeChildren();

	QuadtreeNode* children[4] = { nullptr,nullptr,nullptr,nullptr };
	QuadtreeNode* parent = nullptr;

	std::vector<GameObject*> bucket;

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
	void InsertGameObject(GameObject* gameobject);
	void RemoveGameObject(GameObject* gameobject);

	template<typename PRIMITIVE>
	void CollectIntersections(std::vector<GameObject*>&, const PRIMITIVE & primitive);
	template<typename PRIMITIVE>
	void CollectIntersections(std::map<float, GameObject*>&, const PRIMITIVE & primitive);

	void Draw();
private:
	QuadtreeNode * root = nullptr;
};

template<typename PRIMITIVE>
inline void Quadtree::CollectIntersections(std::vector<GameObject*>& objects, const PRIMITIVE & primitive)
{
	root->CollectIntersections(objects, primitive);
}

template<typename PRIMITIVE>
inline void Quadtree::CollectIntersections(std::map<float, GameObject*>& objects, const PRIMITIVE & primitive)
{
	root->CollectIntersections(objects, primitive);
}

template<typename PRIMITIVE>
inline void QuadtreeNode::CollectIntersections(std::vector<GameObject*>& objects, const PRIMITIVE & primitive)
{
	if (primitive.Intersects(box))
	{
		for (std::vector<GameObject*>::const_iterator it = bucket.begin(); it != bucket.end(); ++it)
		{
			if (primitive.Intersects((*it)->global_aabb))
				objects.push_back(*it);
		}
		for (int i = 0; i < 4; ++i)
			if (children[i] != nullptr) children[i]->CollectIntersections(objects, primitive);
	}
}

template<typename PRIMITIVE>
inline void QuadtreeNode::CollectIntersections(std::map<float, GameObject*>& objects, const PRIMITIVE & primitive)
{
	if (primitive.Intersects(box))
	{
		for (std::vector<GameObject*>::const_iterator it = bucket.begin(); it != bucket.end(); ++it)
		{
			float distance_near, distance_far;
			if (primitive.Intersects((*it)->global_obb, distance_near, distance_far))
				objects.emplace(distance_near, *it);
		}
		for (int i = 0; i < 4; ++i)
			if (children[i] != nullptr) children[i]->CollectIntersections(objects, primitive);
	}
}

#endif // !_QUADTREE_H_
