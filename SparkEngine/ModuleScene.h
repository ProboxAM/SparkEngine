#ifndef _MODULE_SCENE_H_
#define _MODULE_SCENE_H_

#include "Module.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Geometry/LineSegment.h"
#include "MathGeoLib/Geometry/Triangle.h"

class GameObject;
class ResourceModel;
class ComponentCamera;
class Quadtree;
enum PRIMITIVE_TYPE;

#define GRID_SIZE 40
#define DEFAULT_NAME "Scene"

class ModuleScene :
	public Module
{
public:
	GameObject * root = nullptr;
	GameObject* selected_gameobject = nullptr;
	std::map<uint,GameObject*> gameobjects;

	bool global_mode = false;

	Quadtree* quad_tree = nullptr;

public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Init(const nlohmann::json::iterator& it);
	bool Start();
	update_status Update();
	bool CleanUp();

	bool Draw();
	bool DebugDraw();

	bool Load(const nlohmann::json::iterator& it);
	bool Save(nlohmann::json &it);

	bool SaveScene(bool temp = false);
	bool LoadScene(std::string file);

	void OnPlay();
	void OnStop();

	std::string GetName();
	void SetName(std::string);

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
	void DeleteGameObject(GameObject * go);

	void SetGameObjectStatic(GameObject * go, bool state);
	void ResetScene();
	void CreateDefaultScene();
	bool show_grid = true;
	std::vector<float3> lines;

	bool user_selected_GO = false;

	bool HasFile();

	ComponentCamera* main_game_camera = nullptr;

private:
	std::string scene_name = "Scene";
	bool has_file = false;
	void RecursiveErase(GameObject * go);
	void AccelerateFrustumCulling();
};

#endif // !_MODULE_SCENE_H_
