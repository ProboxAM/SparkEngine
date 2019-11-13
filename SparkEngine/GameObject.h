#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include <string>
#include <vector>
#include "Component.h"
#include "MathGeoLib/Geometry/OBB.h"
#include "MathGeoLib/Geometry/AABB.h"
#include "nlohmann\json.hpp"

class ComponentTransform;

class GameObject
{
private:
	std::string name;
	std::string tag = "Untagged";
	std::string layer = "Default";
	bool active = true;
	uint id = -1;
	bool is_static = false;
	std::vector<Component*> components;
public:
	GameObject();
	~GameObject();

	void Update(float dt);
	Component* AddComponent(COMPONENT_TYPE type);
	Component* GetComponent(COMPONENT_TYPE type);
	std::vector<Component*> GetComponents(COMPONENT_TYPE type);
	std::vector<Component*> GetComponents();
	bool HasComponent(COMPONENT_TYPE type);

	void SetName(std::string name);
	std::string GetName();

	void SetTag(std::string tag);
	std::string GetTag();

	bool isActive();
	bool isStatic();
	void SetActive(bool active);
	bool CompareTag(std::string tag);
	bool Save(const nlohmann::json::iterator& it);

	uint GetId();

	void UpdateBBox();

public:
	ComponentTransform * transform = nullptr;
	AABB global_aabb;
	OBB global_obb;
};

#endif // !_GAMEOBJECT_H_


