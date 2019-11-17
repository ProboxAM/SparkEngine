#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "Quadtree.h"



Quadtree::Quadtree()
{
}


Quadtree::~Quadtree()
{
	delete root;
}

void Quadtree::Create(const AABB limits)
{
	root = new QuadtreeNode();
	root->box.SetFrom(limits);
}

void Quadtree::Clear()
{
	root->Clear();
}

void Quadtree::InsertGameObject(const GameObject * gameobject)
{
	if (root->box.Intersects(gameobject->global_aabb))
		root->InsertGameObject(gameobject);
}

void Quadtree::RemoveGameObject(const GameObject * gameobject)
{
	root->RemoveGameObject(gameobject);
}

void Quadtree::Draw()
{
	root->Draw();
}

QuadtreeNode::QuadtreeNode()
{

}

QuadtreeNode::~QuadtreeNode()
{
	for (int i = 0; i < CHILDREN_SIZE; i++) {
		delete children[i];
	}
}

void QuadtreeNode::Create(const AABB limits)
{
	box.SetFrom(limits);
}

void QuadtreeNode::Clear()
{
	for (int i = 0; i < CHILDREN_SIZE; i++) {
		delete children[i];
	}
}

void QuadtreeNode::InsertGameObject(const GameObject * gameobject)
{
	bucket.push_back(gameobject);
	if (bucket.size() > MAX_BUCKET_SIZE) {
		if (!children[0])
			Split();

		DistributeChildren();
	}
}

void QuadtreeNode::RemoveGameObject(const GameObject * gameobject)
{
	for (std::vector<const GameObject*>::iterator it = bucket.begin(); it != bucket.end();) {
		if ((*it) == gameobject)
		{
			it = bucket.erase(it);
			return;
		}
	}

	for (int i = 0; i < CHILDREN_SIZE; i++) {
		children[i]->RemoveGameObject(gameobject);
	}
}

void QuadtreeNode::Draw()
{
	float3 corners[8];
	box.GetCornerPoints(corners);
	App->renderer3D->DebugDrawCube(corners, { 0.0f, 1.0f, 0.0f });

	if (children[0]) {
		for (int i = 0; i < CHILDREN_SIZE; i++) {
			children[i]->Draw();
		}
	}
}

void QuadtreeNode::Split()
{
	AABB child_box;
	float3 center = box.CenterPoint();

	child_box.SetFromCenterAndSize({center.x - (box.HalfSize().x/2), center.y, center.z - (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.Size().y, box.HalfSize().z });
	children[0] = new QuadtreeNode();
	children[0]->Create(child_box);

	child_box.SetFromCenterAndSize({ center.x + (box.HalfSize().x / 2), center.y, center.z - (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.Size().y, box.HalfSize().z });
	children[1] = new QuadtreeNode();
	children[1]->Create(child_box);

	child_box.SetFromCenterAndSize({center.x - (box.HalfSize().x/2), center.y, center.z + (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.Size().y, box.HalfSize().z });
	children[2] = new QuadtreeNode();
	children[2]->Create(child_box);

	child_box.SetFromCenterAndSize({center.x + (box.HalfSize().x/2), center.y, center.z + (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.Size().y, box.HalfSize().z });
	children[3] = new QuadtreeNode();
	children[3]->Create(child_box);
}

void QuadtreeNode::DistributeChildren()
{
	std::vector<uint> intersections;
	for (std::vector<const GameObject*>::iterator it = bucket.begin(); it != bucket.end();) {
		for (int i = 0; i < CHILDREN_SIZE; i++) {
			if ((*it)->global_aabb.Intersects(children[i]->box)) {
				intersections.push_back(i);
			}
		}	
		//object intersects in every child box, so we keep it in the father
		if (intersections.size() == 4)
			++it;
		else
		{
			for each (uint index in intersections)
			{
				children[index]->InsertGameObject((*it));
			}
			it = bucket.erase(it);	
		}
		intersections.clear();
	}
}
