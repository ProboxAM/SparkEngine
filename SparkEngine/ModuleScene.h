#ifndef _MODULE_SCENE_H_
#define _MODULE_SCENE_H_

#include "Module.h"
#include <vector>

class GameObject;
enum PRIMITIVE_TYPE;

#define GRID_SIZE 40

class ModuleScene :
	public Module
{
public:
	GameObject * root = nullptr;
	GameObject* selected_gameobject = nullptr;
	std::vector<GameObject*> gameobjects;
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Init(const nlohmann::json::iterator& it);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Load(const nlohmann::json::iterator& it);
	bool Save(nlohmann::json &it);

	GameObject* CreateGameObject(GameObject* parent = nullptr, std::string name = "GameObject");
	GameObject* CreatePrimitiveGameObject(PRIMITIVE_TYPE type, GameObject* parent = nullptr);
	GameObject* CreateRootGameObject();

	std::vector<float3> grid_points;
	uint grid_id;
	void GenerateGrid();
	void DrawGrid();
	bool show_grid = true;
};

#endif // !_MODULE_SCENE_H_
