#pragma once
#include "Module.h"
#include <vector>

class GameObject;

#define GRID_SIZE 40

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

	GameObject* CreateGameObject(GameObject* parent = nullptr, std::string name = "GameObject");
	GameObject* CreateRootGameObject();

	std::vector<float3> grid_points;
	uint grid_id;
	void GenerateGrid();
	void DrawGrid();
};

