#ifndef _MODULE_SCENE_H_
#define _MODULE_SCENE_H_

#include "Module.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Geometry/LineSegment.h"
#include "MathGeoLib/Geometry/Triangle.h"

class GameObject;
class ResourceModel;
enum PRIMITIVE_TYPE;

#define GRID_SIZE 40

class ModuleScene :
	public Module
{
public:
	GameObject * root = nullptr;
	GameObject* selected_gameobject = nullptr;
	std::map<uint,GameObject*> gameobjects;

	bool global_mode = false;

public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Init(const nlohmann::json::iterator& it);
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Draw();

	bool Load(const nlohmann::json::iterator& it);
	bool Save(nlohmann::json &it);

	bool SaveScene();
	bool LoadScene();

	GameObject* CreateGameObject(
		GameObject* parent = nullptr, 
		std::string name = "GameObject",
		float3 position = {0.0f, 0.0f, 0.0f},
		Quat rotation = Quat::identity,
		float3 scale = {1.0f, 1.0f, 1.0f}	
	);

	GameObject* CreatePrimitiveGameObject(PRIMITIVE_TYPE type, GameObject* parent = nullptr);
	GameObject* CreateRootGameObject();
	GameObject* CreateRootGameObject(uint id);
	GameObject* CreateGameObject(ResourceModel* resource, GameObject* parent = nullptr);
	GameObject* CreateGameObject(GameObject* parent, std::string name, float3 position, Quat rotation, float3 scale, uint id);

	void OnMousePicking(const LineSegment &line);

	std::vector<float3> grid_points;
	uint grid_id;
	void GenerateGrid();
	void DrawGrid();
	void DeleteGameObjects();
	bool show_grid = true;
	std::vector<float3> lines;

	bool user_selected_GO = false;
};

#endif // !_MODULE_SCENE_H_
