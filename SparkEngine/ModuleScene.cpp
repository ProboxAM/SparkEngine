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

	CreatePrimitiveGameObject(P_CUBE);

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

GameObject * ModuleScene::CreatePrimitiveGameObject(PRIMITIVE_TYPE type, GameObject * parent)
{
	std::string name;
	switch (type)
	{
	case P_CUBE:
		name = "Cube";
		break;
	case P_SPHERE:
		name = "Sphere";
		break;
	case P_CYLINDER:
		name = "Cylinder";
		break;
	case P_PLANE:
		name = "Plane";
		break;
	case P_CONE:
		name = "Cone";
		break;
	default:
		name = "GameObject";
		break;
	}

	GameObject* obj = CreateGameObject(parent,name);
	Mesh* mesh = App->meshes->CreatePrimitiveMesh(type);
	ComponentMesh* c_mesh = (ComponentMesh*)obj->AddComponent(COMPONENT_TYPE::MESH);
	c_mesh->AddMesh(mesh);
	ComponentTexture* c_text = (ComponentTexture*)obj->AddComponent(COMPONENT_TYPE::TEXTURE);
	c_text->AddTexture(App->textures->GetDefaultTexture());

	return obj;
}

GameObject * ModuleScene::CreateRootGameObject()
{
	GameObject* go = new GameObject();
	go->transform->SetParent(nullptr);
	go->SetName("root");
	gameobjects.push_back(go);
	return go;
}
