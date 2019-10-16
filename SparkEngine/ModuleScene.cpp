#include "Application.h"
#include "ModuleImporter.h"

#include "GameObject.h"

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentTransform.h"

#include "ModuleMeshes.h"
#include "ModuleTextures.h"

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

	Mesh* mesh = App->meshes->CreatePrimitiveMesh(P_CUBE);
	GameObject* obj = CreateGameObject(nullptr, "Sphere");
	ComponentMesh* c_mesh = (ComponentMesh*)obj->AddComponent(COMPONENT_TYPE::MESH);
	c_mesh->AddMesh(mesh);
	ComponentTexture* c_text = (ComponentTexture*)obj->AddComponent(COMPONENT_TYPE::TEXTURE);
	c_text->AddTexture(App->textures->GetDefaultTexture());

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
