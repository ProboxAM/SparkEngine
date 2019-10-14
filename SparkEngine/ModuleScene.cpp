#include "Application.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "Component.h"
#include "ComponentTransform.h"



ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
	name = "Scene";
}


ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init(nlohmann::json::iterator it)
{
	root = CreateGameObject(nullptr, "root");
	return true;
}

bool ModuleScene::Start()
{
	return true;
}

update_status ModuleScene::Update(float dt)
{
	for (int i = 0; i < gameobjects.size(); i++)
	{
		gameobjects[i]->Update(dt);
	}

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	return true;
}

GameObject * ModuleScene::CreateGameObject(GameObject* parent, std::string name)
{
	GameObject* go = new GameObject();
	go->transform->SetParent(nullptr);
	go->SetName(name);
	gameobjects.push_back(go);
	return go;
}
