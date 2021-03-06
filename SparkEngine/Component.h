#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "nlohmann/json.hpp"

class GameObject;

enum COMPONENT_TYPE { TRANSFORM, MESH, TEXTURE, LIGHT, CAMERA, BONE, ANIMATOR, NONE };

class Component
{
public:
	COMPONENT_TYPE type;
	bool active = true;
	GameObject* gameobject = nullptr;
public:
	Component(GameObject* gameobject);
	virtual ~Component();

	virtual void Enable() { active = true; }
	virtual void Update();
	virtual void Draw() {}
	virtual void Disable() { active = false; }
	virtual bool Save(const nlohmann::json::iterator& it) { return true; }
	virtual bool Load(const nlohmann::json comp) { return true; }
	virtual void OnPlay() {}
};

#endif // !_COMPONENT_H_




