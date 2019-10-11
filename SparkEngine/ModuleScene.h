#pragma once
#include "Module.h"

class GameObject;

class ModuleScene :
	public Module
{
private:
	GameObject* root;
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	GameObject* CreateGameObject();
};

