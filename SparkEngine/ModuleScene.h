#pragma once
#include "Module.h"
#include <vector>

class GameObject;

class ModuleScene :
	public Module
{
private:
	GameObject* root = nullptr;
	std::vector<GameObject*> gameobjects;
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Init(nlohmann::json::iterator it);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	GameObject* CreateGameObject(GameObject* parent = nullptr);
	GameObject* CreateRootGameObject();
};

