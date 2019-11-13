#include "Application.h"
#include "ModuleImporter.h"

#include "GameObject.h"
#include "Mesh.h"

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentTransform.h"

#include "Importer.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "ModuleImporter.h"

#include "ModuleInput.h"
#include "ModuleRenderer3D.h"

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
	Mesh* mesh = App->importer->mesh->LoadPrimitive(type);
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

void ModuleScene::OnMousePicking(const LineSegment &line)
{
	//TODO FIXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	std::vector<GameObject*> candidates;
	for (int i = 0; i < gameobjects.size(); i++) {
		if (gameobjects[i]->HasComponent(COMPONENT_TYPE::MESH)) {
			if (line.Intersects(gameobjects[i]->global_aabb)) {
				float hit_near, hit_far;
				if (line.Intersects(gameobjects[i]->global_obb, hit_near, hit_far))
					candidates.push_back(gameobjects[i]);
			}
		}
	}

	for (int i = 0; i < candidates.size(); i++) {
		ComponentMesh* c_mesh = (ComponentMesh*)candidates[i]->GetComponent(COMPONENT_TYPE::MESH);
		if (c_mesh) {

			LineSegment local = line;
			local.Transform(candidates[i]->transform->GetTransformMatrix().Inverted());

			for (int j = 0; j < c_mesh->GetMesh()->buffers[BUFFER_TYPE::BUFF_IND]; j += 3) {
				uint index_a = c_mesh->GetMesh()->indices[j] * 3;
				float3 a = c_mesh->GetMesh()->vertices[index_a];

				uint index_b = c_mesh->GetMesh()->indices[j + 1] * 3;
				float3 b = c_mesh->GetMesh()->vertices[index_b];

				uint index_c = c_mesh->GetMesh()->indices[j + 2] * 3;
				float3 c = c_mesh->GetMesh()->vertices[index_c];

				Triangle t(a, b, c);

				if (local.Intersects(t, nullptr, nullptr)) {
					selected_gameobject = candidates[i];
					LOG("%s", selected_gameobject->GetName().c_str());
				}
			}
		}
	}
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