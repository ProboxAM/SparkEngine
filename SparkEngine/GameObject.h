#pragma once
#include <string>
#include <vector>
#include "Component.h"

class ComponentTransform;

class GameObject
{
private:
	std::string name;
	std::string tag;
	std::string layer;
	bool active;
	bool is_static;
	std::vector<Component*> components;
public:
	GameObject();
	~GameObject();

	void Update(float dt);
	Component* AddComponent(COMPONENT_TYPE type);
	Component* GetComponent(COMPONENT_TYPE type);
	std::vector<Component*> GetComponents(COMPONENT_TYPE type);
	bool isActive();
	bool isStatic();
	void SetActive(bool active);
	bool CompareTag(std::string tag);

public:
	ComponentTransform* transform = nullptr;
};

