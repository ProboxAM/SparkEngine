#include "Application.h"
#include "ModuleImporter.h"
#include "ModuleResources.h"

#include "GameObject.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceModel.h"

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentTransform.h"

#include "Importer.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "ModuleImporter.h"

#include "ModuleInput.h"
#include "ModuleScene.h"

#include "glew/glew.h"

#include <fstream>
#include <iomanip>

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
	name = "Scene";
}


ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init(const nlohmann::json::iterator& it)
{
	LOG("Init Scene");

	LOG("Creating root game object");
	root = CreateRootGameObject();

	show_grid = (*it)["grid"];

	return true;
}

bool ModuleScene::Start()
{
	App->importer->ImportFile("Assets/Bakerhouse.fbx");

	GenerateGrid();

	return true;
}

update_status ModuleScene::UpdateScene(float dt)
{
	if(show_grid)
		DrawGrid();

	root->Update(dt);

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveScene();

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		ResourceModel* res = (ResourceModel*) App->resources->Get(8);
		CreateGameObject(res, root);
	}

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	return true;
}

bool ModuleScene::Load(const nlohmann::json::iterator& it)
{
	show_grid = (*it)["grid"];

	return true;
}
bool ModuleScene::Save(nlohmann::json& it)
{
	it[name] = {
		{ "grid", show_grid },
	};

	return true;
}

bool ModuleScene::SaveScene()
{
	LOG("Saving scene...");

	nlohmann::json j = {
		{"GameObjects", nlohmann::json::array()}
	};

	root->Save(j.find("GameObjects"));

	std::ofstream o(ASSETS_FOLDER"Scene.scene");
	o << std::setw(4) << j << std::endl;

	LOG("Scene saved to %s", ASSETS_FOLDER"Scene.scene");

	return true;
}

GameObject * ModuleScene::CreateGameObject(GameObject * parent, std::string name, float3 position, Quat rotation, float3 scale)
{
	if (!parent)
		parent = root;

	GameObject* go = new GameObject();
	go->transform->SetParent(parent->transform);
	parent->transform->AddChild(go->transform);
	go->SetName(name);
	go->transform->local_position = position;
	go->transform->local_rotation = rotation;
	go->transform->local_scale = scale;
	go->transform->local_euler_rotation = rotation.ToEulerXYZ()*RADTODEG;
	gameobjects.push_back(go);
	go->transform->UpdateTransformMatrix();
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

	GameObject* obj = CreateGameObject(parent, name);
	ResourceMesh* mesh = App->importer->mesh->LoadPrimitive(type);
	ComponentMesh* c_mesh = (ComponentMesh*)obj->AddComponent(COMPONENT_TYPE::MESH);
	c_mesh->AddMesh(mesh);
	ComponentTexture* c_text = (ComponentTexture*)obj->AddComponent(COMPONENT_TYPE::TEXTURE);
	c_text->AddTexture(App->importer->texture->LoadDefault());

	return obj;
}

GameObject * ModuleScene::CreateRootGameObject()
{
	GameObject* go = new GameObject();
	go->SetName("root");
	gameobjects.push_back(go);
	return go;
}

GameObject * ModuleScene::CreateGameObject(ResourceModel * resource, GameObject* parent)
{
	std::vector<GameObject*> temp_go;
	uint count = 0;

	for each (ResourceModel::ModelNode node in resource->nodes)
	{
		GameObject* go = new GameObject();

		if (count > 0)
		{
			go->transform->SetParent(temp_go[node.parent]->transform);
			temp_go[node.parent]->transform->AddChild(go->transform);
		}
		else
		{
			go->transform->SetParent(parent->transform);
			parent->transform->AddChild(go->transform);
		}		

		go->SetName(node.name);
		go->transform->local_position = node.position;
		go->transform->local_rotation = node.rotation;
		go->transform->local_scale = node.scale;
		go->transform->local_euler_rotation = node.rotation.ToEulerXYZ()*RADTODEG;

		if (node.mesh > 0)
		{
			ComponentMesh* c_mesh = (ComponentMesh*)go->AddComponent(COMPONENT_TYPE::MESH);
			c_mesh->AddMesh((ResourceMesh*)App->resources->Get(node.mesh));
		}
		if (node.texture > 0)
		{
			ComponentTexture* c_text = (ComponentTexture*)go->AddComponent(COMPONENT_TYPE::TEXTURE);
			c_text->AddTexture((ResourceTexture*)App->resources->Get(node.texture));
		}

		gameobjects.push_back(go);
		temp_go.push_back(go);
		go->transform->UpdateTransformMatrix();
		count++;
	}

	return temp_go[0];
}

void ModuleScene::GenerateGrid()
{
	float start = GRID_SIZE / 2;
	float step_horizontal = start + 1;
	float step_vertical = start + 1;
	for (int i = 0; i < GRID_SIZE + 1; i++)
	{
		grid_points.push_back({ start, 0.f, step_horizontal -= 1 });
		grid_points.push_back({ -start, 0.f, step_horizontal });
		grid_points.push_back({ step_vertical -= 1, 0.f, start });
		grid_points.push_back({ step_vertical, 0.f, -start });
	}

	glGenBuffers(1, (GLuint*)&grid_id);

	glBindBuffer(GL_ARRAY_BUFFER, grid_id);
	glBufferData(GL_ARRAY_BUFFER, grid_points.size() * sizeof(float3), &grid_points[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void ModuleScene::DrawGrid()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, grid_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_LINES, 0, grid_points.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}