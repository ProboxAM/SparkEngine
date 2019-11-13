#ifndef _MODULE_SCENE_H_
#define _MODULE_SCENE_H_

#include "Module.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Geometry/LineSegment.h"
#include "MathGeoLib/Geometry/Triangle.h"
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
	update_status UpdateScene(float dt);
	bool CleanUp();

	bool Load(const nlohmann::json::iterator& it);
	bool Save(nlohmann::json &it);

	bool SaveScene();

	GameObject* CreateGameObject(
		GameObject* parent = nullptr, 
		std::string name = "GameObject",
		float3 position = {0.0f, 0.0f, 0.0f},
		Quat rotation = Quat::identity,
		float3 scale = {1.0f, 1.0f, 1.0f}	
	);
	GameObject* CreatePrimitiveGameObject(PRIMITIVE_TYPE type, GameObject* parent = nullptr);
	GameObject* CreateRootGameObject();

	void OnMousePicking(const LineSegment &line);

	std::vector<float3> grid_points;
	uint grid_id;
	void GenerateGrid();
	void DrawGrid();
	bool show_grid = true;
	std::vector<float3> lines;

};

#endif // !_MODULE_SCENE_H_
