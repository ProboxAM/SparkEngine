#pragma once
#include <string>
#include <vector>
#include "Component.h"

class GameObject
{
private:
	std::string name;
	std::vector<Component*> components;
	GameObject* parent;
	std::vector<GameObject*> children;
public:

	void Update(float dt);
	Component* CreateComponent(COMPONENT_TYPE type);
	GameObject();
	~GameObject();
};

