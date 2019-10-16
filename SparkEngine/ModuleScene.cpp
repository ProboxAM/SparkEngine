#include "Application.h"
#include "ModuleImporter.h"

#include "GameObject.h"

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentTransform.h"

#include "ModuleScene.h"

#include "glew/glew.h"

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

	GenerateGrid();

	return true;
}

update_status ModuleScene::Update(float dt)
{
	DrawGrid();

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
