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
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "PanelProject.h"
#include "Quadtree.h"

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

	quad_tree = new Quadtree();
	quad_tree->Create(AABB(float3(-80, -30, -80), float3(80, 30, 80)));

	return true;
}

update_status ModuleScene::Update(float dt)
{
	root->Update(dt);

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveScene();
	else if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		LoadScene();

	if (App->input->GetKey(SDL_SCANCODE_DELETE) && selected_gameobject)
	{
		DeleteGameObject(selected_gameobject);
		selected_gameobject = nullptr;
	}


	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	return true;
}

bool ModuleScene::Draw()
{
	if (show_grid)
		DrawGrid();

	root->Draw();

	quad_tree->Draw();

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

	std::string scene_path = ASSETS_FOLDER + scene_name + SCENE_EXTENSION;
	std::ofstream o(scene_path);
	o << std::setw(4) << j << std::endl;

	LOG("Scene saved to %s", scene_path.c_str());

	has_file = true;

	return true;
}

bool ModuleScene::LoadScene()
{
	LOG("Loading scene...");

	DeleteGameObjects();

	std::ifstream i(ASSETS_FOLDER"Scene.scene");
	nlohmann::json j = nlohmann::json::parse(i);

	nlohmann::json game_objects = j.find("GameObjects").value();
	for (nlohmann::json::iterator it = game_objects.begin(); it != game_objects.end(); ++it)
	{
		nlohmann::json object = it.value();
		std::string name = object["name"];
		uint parent_id = object["parent"];
		uint id = object["id"];

		if (parent_id != 0)
		{
			float3 position, scale;
			Quat rotation;
			
			//TODO: Clean this ////////////////////////
			position = float3(object["components"][COMPONENT_TYPE::TRANSFORM]["position"][0], object["components"][COMPONENT_TYPE::TRANSFORM]["position"][1],
				object["components"][COMPONENT_TYPE::TRANSFORM]["position"][2]);
			rotation = Quat(object["components"][COMPONENT_TYPE::TRANSFORM]["rotation"][0], object["components"][0]["rotation"][1],
				object["components"][COMPONENT_TYPE::TRANSFORM]["rotation"][2], object["components"][COMPONENT_TYPE::TRANSFORM]["rotation"][3]);
			scale = float3(object["components"][COMPONENT_TYPE::TRANSFORM]["scale"][0], object["components"][COMPONENT_TYPE::TRANSFORM]["scale"][1],
				object["components"][COMPONENT_TYPE::TRANSFORM]["scale"][2]);
			///////////////////////////////////////////

			GameObject* parent_go = gameobjects.find(parent_id)->second;
			GameObject* go = CreateGameObject(parent_go, name, position, rotation, scale, id);

			for (nlohmann::json::iterator components_it = object["components"].begin(); components_it != object["components"].end(); ++components_it)
			{
				nlohmann::json component = components_it.value();

				if (component["type"] == COMPONENT_TYPE::TRANSFORM)
					continue;

				Component* comp = go->AddComponent(component["type"]);
				comp->Load(component);
			}
		}
		else
		{
			root = CreateRootGameObject(id);
		}
	}
	LOG("Finished loading scene.");

	has_file = true;

	return true;
}

std::string ModuleScene::GetName()
{
	return scene_name;
}

void ModuleScene::SetName(std::string new_name)
{
	scene_name = new_name;
}

GameObject * ModuleScene::CreateGameObject(GameObject* parent, std::string name, float3 position, Quat rotation, float3 scale)
{
	if (!parent)
		parent = root;

	GameObject* go = new GameObject();
	go->transform->SetParent(parent->transform);
	parent->transform->AddChild(go->transform);
	go->SetName(name);
	go->transform->SetLocalPosition(position);
	go->transform->SetLocalRotation(rotation);
	go->transform->SetLocalScale(scale);

	gameobjects.emplace(go->GetId(), go);
	go->transform->UpdateTransformMatrix();

	return go;
}

GameObject * ModuleScene::CreateGameObject(GameObject* parent, std::string name, float3 position, Quat rotation, float3 scale, uint id)
{
	if (!parent)
		parent = root;

	GameObject* go = new GameObject(id);
	go->transform->SetParent(parent->transform);
	parent->transform->AddChild(go->transform);
	go->SetName(name);
	go->transform->SetLocalPosition(position);
	go->transform->SetLocalRotation(rotation);
	go->transform->SetLocalScale(scale);

	gameobjects.emplace(go->GetId(), go);
	go->transform->UpdateTransformMatrix();

	return go;
}

GameObject* ModuleScene::CreatePrimitiveGameObject(PRIMITIVE_TYPE type, GameObject * parent)
{
	std::string name;
	uint shape_id;
	switch (type)
	{
	case P_CUBE:
		name = "Cube";
		shape_id = App->importer->mesh->cube;
		break;
	case P_SPHERE:
		name = "Sphere";
		shape_id = App->importer->mesh->sphere;
		break;
	case P_CYLINDER:
		name = "Cylinder";
		shape_id = App->importer->mesh->cylinder;
		break;
	case P_PLANE:
		name = "Plane";
		shape_id = App->importer->mesh->plane;
		break;
	case P_CONE:
		name = "Cone";
		shape_id = App->importer->mesh->cone;
		break;
	default:
		name = "GameObject";
		shape_id = App->importer->mesh->cube;
		break;
	}

	GameObject* obj = CreateGameObject(parent, name);

	ComponentMesh* c_mesh = (ComponentMesh*)obj->AddComponent(COMPONENT_TYPE::MESH);
	c_mesh->AddMesh((ResourceMesh*)App->resources->GetAndLoad(shape_id));

	ComponentTexture* c_text = (ComponentTexture*)obj->AddComponent(COMPONENT_TYPE::TEXTURE);
	c_text->AddTexture((ResourceTexture*)App->resources->GetAndLoad(App->importer->texture->checkers));

	return obj;
}

GameObject * ModuleScene::CreateRootGameObject()
{
	GameObject* go = new GameObject();
	go->SetName("root");
	gameobjects.emplace(go->GetId(), go);
	return go;
}

GameObject * ModuleScene::CreateRootGameObject(uint id)
{
	GameObject* go = new GameObject(id);
	go->SetName("root");
	gameobjects.emplace(go->GetId(), go);

	return go;
}

GameObject * ModuleScene::CreateGameObject(ResourceModel * resource, GameObject* parent)
{
	if (parent == nullptr)
		parent = root;
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
		go->transform->SetLocalPosition(node.position);
		go->transform->SetLocalRotation(node.rotation);
		go->transform->SetLocalScale(node.scale);

		if (node.mesh > 0)
		{
			ComponentMesh* c_mesh = (ComponentMesh*)go->AddComponent(COMPONENT_TYPE::MESH);
			c_mesh->AddMesh((ResourceMesh*)App->resources->GetAndLoad(node.mesh));

			ComponentTexture* c_text = (ComponentTexture*)go->AddComponent(COMPONENT_TYPE::TEXTURE);
			if (node.texture > 0)
				c_text->AddTexture((ResourceTexture*)App->resources->GetAndLoad(node.texture));
			else
				c_text->AddTexture((ResourceTexture*)App->resources->GetAndLoad(App->importer->texture->checkers));
		}


		gameobjects.emplace(go->GetId(), go);
		temp_go.push_back(go);
		go->transform->UpdateTransformMatrix();
		count++;
	}
	return temp_go[0];
}

void ModuleScene::OnMousePicking(const LineSegment &line)
{
	std::map<float, GameObject*> objects_hit;
	quad_tree->CollectIntersections(objects_hit, line);

	for (std::map<uint, GameObject*>::iterator it = gameobjects.begin(); it != gameobjects.end(); ++it)
	{
		if (!it->second->isStatic()) {
			if (it->second->HasComponent(COMPONENT_TYPE::MESH)) {
				if (line.Intersects(it->second->global_aabb)) {
					float distance_near, distance_far;
					if (line.Intersects(it->second->global_obb, distance_near, distance_far)) {
						objects_hit.emplace(distance_near, it->second);
					}
				}
			}
		}
	}

	for (std::map<float, GameObject*>::iterator it = objects_hit.begin(); it != objects_hit.end(); ++it) {
		ComponentMesh* c_mesh = (ComponentMesh*)it->second->GetComponent(COMPONENT_TYPE::MESH);
		if (c_mesh) {

			LineSegment local = line;
			local.Transform(it->second->transform->GetTransformMatrix().Inverted());

			for (int j = 0; j < c_mesh->GetMesh()->total_indices; j += 3) {
				uint index_a = c_mesh->GetMesh()->indices[j];
				float3 a = c_mesh->GetMesh()->vertices[index_a];

				uint index_b = c_mesh->GetMesh()->indices[j + 1];
				float3 b = c_mesh->GetMesh()->vertices[index_b];

				uint index_c = c_mesh->GetMesh()->indices[j + 2];
				float3 c = c_mesh->GetMesh()->vertices[index_c];

				Triangle t(a, b, c);

				if (local.Intersects(t, nullptr, nullptr)) {
					selected_gameobject = it->second;
					user_selected_GO = true;

					PanelProject* panel_project = (PanelProject*)App->editor->GetPanel(Panel_Type::PROJECT);
					panel_project->selected_file = "";
					panel_project->selected_resource = nullptr;

					return;
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

void ModuleScene::DeleteGameObjects()
{
	root->Delete();
	gameobjects.clear();
	root = nullptr;
}

void ModuleScene::DeleteGameObject(GameObject* go)
{
	go->transform->GetParent()->RemoveChild(go->transform);
	RecursiveErase(go);
	go->Delete();

	go = nullptr;
}

bool ModuleScene::HasFile()
{
	return has_file;
}

void ModuleScene::RecursiveErase(GameObject* go)
{
	std::vector<ComponentTransform*> children = go->transform->GetChildren();
	for each (ComponentTransform* child in children)
	{
		RecursiveErase(child->gameobject);
	}
	gameobjects.erase(go->GetId());
}

void ModuleScene::SetGameObjectStatic(GameObject* go, bool state)
{
	go->SetStatic(state);
	if (state)
		quad_tree->InsertGameObject(go);
	else
		quad_tree->RemoveGameObject(go);
}

void ModuleScene::ResetScene()
{
	DeleteGameObjects();
	quad_tree->Clear();
	scene_name = DEFAULT_NAME;

	root = CreateRootGameObject();
	has_file = false;
}