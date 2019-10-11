#pragma once

class GameObject;

enum COMPONENT_TYPE { TRANSFORM, MESH, MATERIAL, LIGHT };

class Component
{
protected:
	COMPONENT_TYPE type;
	bool active;
	GameObject* my_go;
public:
	Component(GameObject* my_go);
	~Component();

	virtual void Enable() { active = true; }
	virtual void Update();
	virtual void Disable() { active = false; }
};

