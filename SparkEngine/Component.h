#ifndef _COMPONENT_H_
#define _COMPONENT_H_

class GameObject;

enum COMPONENT_TYPE { TRANSFORM, MESH, TEXTURE, LIGHT, NONE };

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

#endif // !_COMPONENT_H_




