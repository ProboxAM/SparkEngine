#pragma once

class GameObject;

enum COMPONENT_TYPE { TRANSFORM, MESH, TEXTURE, LIGHT };

class Component
{
public:
	COMPONENT_TYPE type;
	bool active;
	GameObject* gameobject;
public:
	Component(GameObject* gameobject);
	~Component();

	virtual void Enable() { active = true; }
	virtual void Update(float dt);
	virtual void Disable() { active = false; }
};

