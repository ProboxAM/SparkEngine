#include "Application.h"
#include "ModuleImporter.h"

#include "GameObject.h"

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentTransform.h"

#include "ModuleScene.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
	name = "Scene";
}


ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init(nlohmann::json::iterator it)
{
	root = CreateRootGameObject();
	return true;
}

bool ModuleScene::Start()
{
	App->importer->ImportFile("Assets/Bakerhouse.fbx");

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
	if (!parent)
		parent = root;

	GameObject* go = new GameObject();
	go->transform->SetParent(parent->transform);
	go->SetName(name);
	gameobjects.push_back(go);
	return go;
}

GameObject * ModuleScene::CreateRootGameObject()
{
	GameObject* go = new GameObject();
	go->transform->SetParent(nullptr);
	go->SetName("root");
	gameobjects.push_back(go);
	return go;
}
