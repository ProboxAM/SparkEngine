#pragma once
#include "Module.h"
#include <vector>

class GameObject;

class ModuleScene :
	public Module
{
public:
	GameObject* root = nullptr;
	GameObject* selected_gameobject = nullptr;
	std::vector<GameObject*> gameobjects;
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Init(nlohmann::json::iterator it);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	GameObject* CreateGameObject(GameObject* parent, std::string name);
};

