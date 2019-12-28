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
	uint id = 0;
	bool is_static = false;

	std::vector<Component*> components;
public:
	GameObject();
	GameObject(uint id);
	~GameObject();

	void Update();
	void Delete();
	void Draw();
	Component* AddComponent(COMPONENT_TYPE type);
	Component* GetComponent(COMPONENT_TYPE type) const;
	std::vector<Component*> GetComponentsByType(COMPONENT_TYPE type) const;
	std::vector<Component*> GetComponents() const;
	bool HasComponent(COMPONENT_TYPE type) const;

	void SetName(const std::string &name);
	std::string GetName() const;

	void SetTag(const std::string &tag);
	std::string GetTag() const;

	bool isActive() const;
	bool isStatic() const;
	void OnPlay();
	void SetStatic(bool state);
	void SetActive(bool active);
	bool CompareTag(const std::string &tag);
	bool Save(const nlohmann::json::iterator& it);

	float GetDistanceFromAABB() const;

	uint GetId() const;

	void UpdateBBox();

public:
	ComponentTransform * transform = nullptr;
	AABB aabb;
	OBB obb;
};

#endif // !_GAMEOBJECT_H_


