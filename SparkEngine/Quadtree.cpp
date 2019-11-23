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

void Quadtree::Rebuild()
{
	std::vector<GameObject*>to_rebuild;

	root->Rebuild(to_rebuild);
	AABB box = root->box;
	root->Clear();

	Create(box);

	for (int i = 0; i < to_rebuild.size(); i++) {
		InsertGameObject(to_rebuild[i]);
	}
}

void Quadtree::InsertGameObject(GameObject * gameobject)
{
	if (root->box.Intersects(gameobject->aabb))
		root->InsertGameObject(gameobject);
}

void Quadtree::RemoveGameObject(GameObject * gameobject)
{
	root->RemoveGameObject(gameobject);
	Rebuild();
}

void Quadtree::Draw()
{
	if(App->renderer3D->debug_draw && debug_draw_tree)
		root->Draw();
}

QuadtreeNode::QuadtreeNode()
{
	for (int i = 0; i < CHILDREN_SIZE; i++) {
		children[i] = nullptr;
	}
}

QuadtreeNode::~QuadtreeNode()
{

}

void QuadtreeNode::Create(const AABB limits)
{
	box.SetFrom(limits);
}

void QuadtreeNode::Clear()
{
	for (std::vector<GameObject*>::const_iterator it = bucket.begin(); it != bucket.end();) {
		it = bucket.erase(it);
	};

	bucket.clear();

	if (children[0]) {
		for (int i = 0; i < CHILDREN_SIZE; i++) {
			children[i]->Clear();
		}
	}

	delete this;
}

void QuadtreeNode::Rebuild(std::vector<GameObject*> &to_rebuild)
{
	for (std::vector<GameObject*>::const_iterator it = bucket.begin(); it != bucket.end(); ++it) {
		bool find = false;
		for (int i = 0; i < to_rebuild.size(); i++) {
			if (to_rebuild[i] == (*it)) {
				find = true;
				break;
			}
		}
		if(!find)to_rebuild.push_back((*it));
	}

	if (children[0]) {
		for (int i = 0; i < CHILDREN_SIZE; i++) {
			children[i]->Rebuild(to_rebuild);
		}
	}
}


void QuadtreeNode::InsertGameObject(GameObject * gameobject)
{
	if(gameobject->HasComponent(COMPONENT_TYPE::MESH))bucket.push_back(gameobject);
	if (bucket.size() > MAX_BUCKET_SIZE && layer < MAX_DIVISIONS) {
		if (!children[0])
			Split();

		DistributeChildren();
	}
}

void QuadtreeNode::RemoveGameObject(GameObject * gameobject)
{
	for (std::vector<GameObject*>::const_iterator it = bucket.begin(); it != bucket.end(); ++it) {
		if ((*it) == gameobject)
		{
			it = bucket.erase(it);
			return;
		}
	}

	if (children[0]) {
		for (int i = 0; i < CHILDREN_SIZE; i++) {
			children[i]->RemoveGameObject(gameobject);
		}
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

	child_box.SetFromCenterAndSize({center.x - (box.HalfSize().x / 2), center.y + (box.HalfSize().y / 2), center.z - (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.HalfSize().y, box.HalfSize().z });
	children[0] = new QuadtreeNode();
	children[0]->Create(child_box);
	children[0]->layer = layer + 1;

	child_box.SetFromCenterAndSize({ center.x + (box.HalfSize().x / 2), center.y + (box.HalfSize().y / 2), center.z - (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.HalfSize().y, box.HalfSize().z });
	children[1] = new QuadtreeNode();
	children[1]->Create(child_box);
	children[1]->layer = layer + 1;

	child_box.SetFromCenterAndSize({center.x - (box.HalfSize().x/2), center.y + (box.HalfSize().y / 2), center.z + (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.HalfSize().y, box.HalfSize().z });
	children[2] = new QuadtreeNode();
	children[2]->Create(child_box);
	children[2]->layer = layer + 1;

	child_box.SetFromCenterAndSize({center.x + (box.HalfSize().x/2), center.y + (box.HalfSize().y / 2), center.z + (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.HalfSize().y, box.HalfSize().z });
	children[3] = new QuadtreeNode();
	children[3]->Create(child_box);
	children[3]->layer = layer + 1;

	child_box.SetFromCenterAndSize({ center.x - (box.HalfSize().x / 2), center.y - (box.HalfSize().y / 2), center.z - (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.HalfSize().y, box.HalfSize().z });
	children[4] = new QuadtreeNode();
	children[4]->Create(child_box);
	children[4]->layer = layer + 1;

	child_box.SetFromCenterAndSize({ center.x + (box.HalfSize().x / 2), center.y - (box.HalfSize().y / 2), center.z - (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.HalfSize().y, box.HalfSize().z });
	children[5] = new QuadtreeNode();
	children[5]->Create(child_box);
	children[5]->layer = layer + 1;

	child_box.SetFromCenterAndSize({ center.x - (box.HalfSize().x / 2), center.y - (box.HalfSize().y / 2), center.z + (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.HalfSize().y, box.HalfSize().z });
	children[6] = new QuadtreeNode();
	children[6]->Create(child_box);
	children[6]->layer = layer + 1;

	child_box.SetFromCenterAndSize({ center.x + (box.HalfSize().x / 2), center.y - (box.HalfSize().y / 2), center.z + (box.HalfSize().z / 2) },
		{ box.HalfSize().x ,box.HalfSize().y, box.HalfSize().z });
	children[7] = new QuadtreeNode();
	children[7]->Create(child_box);
	children[7]->layer = layer + 1;
}

void QuadtreeNode::DistributeChildren()
{
	std::vector<uint> intersections;
	for (std::vector<GameObject*>::const_iterator it = bucket.begin(); it != bucket.end();) {
		for (int i = 0; i < CHILDREN_SIZE; i++) {
			if ((*it)->aabb.Intersects(children[i]->box)) {
				intersections.push_back(i);
			}
		}	
		//object intersects in every child box, so we keep it in the father
		if (intersections.size() == CHILDREN_SIZE)
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
