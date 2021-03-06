#include "Application.h"
#include "ModuleImporter.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceModel.h"
#include "ResourceBone.h"

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ComponentBone.h"

#include "Importer.h"
#include "MeshImporter.h"
#include "TextureImporter.h"
#include "ModuleImporter.h"

#include "ModuleInput.h"
#include "ModuleCamera3D.h"
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

	LOG("Creating scene");
	CreateDefaultScene();

	show_grid = (*it)["grid"];

	return true;
}

bool ModuleScene::Start()
{
	LoadScene(ASSETS_FOLDER + std::string("Scenes/DefaultScene") + SCENE_EXTENSION);

	GenerateGrid();

	return true;
}

update_status ModuleScene::Update()
{
	root->Update();

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->editor->SaveScene();
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
		ResetScene();

	if (App->input->GetKey(SDL_SCANCODE_DELETE) && selected_gameobject)
	{
		DeleteGameObject(selected_gameobject);
		selected_gameobject = nullptr;
		ImGuizmo::SetRect(0, 0, 0, 0);
	}

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	return true;
}

bool ModuleScene::Draw()
{
	root->Draw();

	return true;
}

bool ModuleScene::DebugDraw()
{
	if (show_grid)
		DrawGrid();
	
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

bool ModuleScene::SaveScene(bool temp)
{
	LOG("Saving scene...");

	nlohmann::json j = {
		{"GameObjects", nlohmann::json::array()}
	};

	root->Save(j.find("GameObjects"));

	bool new_file = false;
	std::string scene_path;
	if(temp)
		scene_path = SETTINGS_FOLDER + std::string("tmp_scene") + SCENE_EXTENSION;
	else
	{
		if (file.empty())
		{
			file = App->editor->GetProjectPanelPath() + scene_name + SCENE_EXTENSION;
			root->SetName(scene_name);
			new_file = true;
		}		
		scene_path = file;
	}

	std::ofstream o(scene_path);
	o << std::setw(4) << j << std::endl;
	if (new_file)
		App->editor->ReloadProjectWindow();
	LOG("Scene saved to %s", scene_path.c_str());

	return true;
}

bool ModuleScene::LoadScene(std::string file, bool temp)
{
	LOG("Loading scene %s", file.c_str());

	if (!App->fsystem->Exists(file.c_str()))
	{
		LOG("Cant find scene file.");
		return false;
	}

	if (!temp)
	{
		this->file = file;
		App->fsystem->SplitFilePath(file.c_str(), nullptr, &scene_name);
	}


	DeleteGameObjects();

	std::ifstream i(file);
	nlohmann::json j = nlohmann::json::parse(i);
	std::vector<ComponentMesh*> loaded_meshes;

	nlohmann::json game_objects = j.find("GameObjects").value();
	for (nlohmann::json::iterator it = game_objects.begin(); it != game_objects.end(); ++it)
	{
		nlohmann::json object = it.value();
		std::string name = object["name"];
		uint parent_id = object["parent"];
		uint id = object["id"];
		bool is_static = object["static"];

		if (parent_id != 0)
		{
			GameObject* parent_go = gameobjects.find(parent_id)->second;
			GameObject* go = CreateGameObject(parent_go, name, float3::zero, Quat::identity, float3::one, id);

			for (nlohmann::json::iterator components_it = object["components"].begin(); components_it != object["components"].end(); ++components_it)
			{
				Component* comp;
				nlohmann::json component = components_it.value();
				COMPONENT_TYPE type = (COMPONENT_TYPE)component["type"];

				if (type != COMPONENT_TYPE::TRANSFORM)
					comp = go->AddComponent(type);
				else
					comp = go->transform;

				comp->Load(component);

				if (type == COMPONENT_TYPE::MESH)
					loaded_meshes.push_back((ComponentMesh*)comp);
			}

			if (is_static)
				SetGameObjectStatic(go, true);
		}
		else
		{
			root = CreateRootGameObject(id);
		}
	}

	for each (ComponentMesh* c_mesh in loaded_meshes)
		c_mesh->AttachSkeleton();

	LOG("Finished loading scene.");

	return true;
}

void ModuleScene::OnPlay()
{
	SaveScene(true);

	root->OnPlay();
}

void ModuleScene::OnStop()
{
	LoadScene(SETTINGS_FOLDER + std::string("tmp_scene") + SCENE_EXTENSION, true);
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

GameObject* ModuleScene::CreateGameObject(GameObject* parent, const std::string &name, const float3 &position, const Quat &rotation, const float3 &scale, uint id)
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

GameObject * ModuleScene::CreateGameObject(ResourceMesh * resource)
{
	GameObject* go = new GameObject();
	go->transform->SetParent(root->transform);
	root->transform->AddChild(go->transform);
	go->SetName(std::to_string(resource->GetID()).c_str());

	ComponentMesh* c_mesh = (ComponentMesh*)go->AddComponent(COMPONENT_TYPE::MESH);
	c_mesh->AddMesh((ResourceMesh*)App->resources->GetAndReference(resource->GetID()));

	ComponentTexture* c_text = (ComponentTexture*)go->AddComponent(COMPONENT_TYPE::TEXTURE);
	c_text->AddTexture((ResourceTexture*)App->resources->GetAndReference(App->importer->texture->checkers));

	gameobjects.emplace(go->GetId(), go);
	go->transform->UpdateTransformMatrix();
	return nullptr;
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
	c_mesh->AddMesh((ResourceMesh*)App->resources->GetAndReference(shape_id));

	ComponentTexture* c_text = (ComponentTexture*)obj->AddComponent(COMPONENT_TYPE::TEXTURE);
	c_text->AddTexture((ResourceTexture*)App->resources->GetAndReference(App->importer->texture->checkers));

	return obj;
}

GameObject * ModuleScene::CreateRootGameObject()
{
	GameObject* go = new GameObject();
	go->SetName(scene_name);
	gameobjects.emplace(go->GetId(), go);
	return go;
}

GameObject * ModuleScene::CreateRootGameObject(uint id)
{
	GameObject* go = new GameObject(id);
	go->SetName(scene_name);
	gameobjects.emplace(go->GetId(), go);

	return go;
}

GameObject * ModuleScene::CreateGameObject(ResourceModel * resource, GameObject* parent)
{
	LOG("Creating GameObject from resource %s", resource->GetFile());
	if (parent == nullptr)
		parent = root;

	std::vector<GameObject*> temp_go;
	std::vector<ComponentMesh*> temp_meshes;
	std::map<ComponentBone*, uint> temp_root_bones;

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
			c_mesh->AddMesh((ResourceMesh*)App->resources->GetAndReference(node.mesh));
			temp_meshes.push_back(c_mesh);

			ComponentTexture* c_text = (ComponentTexture*)go->AddComponent(COMPONENT_TYPE::TEXTURE);
			if (node.texture > 0)
				c_text->AddTexture((ResourceTexture*)App->resources->GetAndReference(node.texture));
			else
				c_text->AddTexture((ResourceTexture*)App->resources->GetAndReference(App->importer->texture->checkers));
		}
		if (node.bone > 0)
		{
			ComponentBone* c_bone = (ComponentBone*)go->AddComponent(COMPONENT_TYPE::BONE);
			c_bone->AddBone((ResourceBone*)App->resources->GetAndReference(node.bone));
			if (node.root_bone)
				temp_root_bones.emplace(c_bone, node.mesh_binded);
		}

		gameobjects.emplace(go->GetId(), go);
		temp_go.push_back(go);
		go->transform->UpdateTransformMatrix();
		count++;
	}

	//Link root bones to each mesh component
	for(std::map<ComponentBone*,uint>::iterator it = temp_root_bones.begin(); it!=temp_root_bones.end(); ++it)
	{
		for each (ComponentMesh* c_mesh in temp_meshes)
		{
			if (it->second == c_mesh->GetMesh()->GetID())
				c_mesh->AttachSkeleton(it->first->gameobject->transform);
		}
	}

	return temp_go[0];
}

GameObject* ModuleScene::OnMousePicking(const LineSegment &line)
{
	std::map<float, GameObject*> objects_hit;
	quad_tree->CollectIntersections(objects_hit, line);

	for (std::map<uint, GameObject*>::iterator it = gameobjects.begin(); it != gameobjects.end(); ++it)
	{
		if (!it->second->isStatic()) {
			if (it->second->HasComponent(COMPONENT_TYPE::MESH)) {
				if (line.Intersects(it->second->aabb)) {
					float distance_near, distance_far;
					if (line.Intersects(it->second->obb, distance_near, distance_far)) {
						objects_hit.emplace(distance_near, it->second);
					}
				}
			}
		}
	}

	if (objects_hit.size() == 0) {
		selected_gameobject = nullptr;
		return nullptr;
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
					return it->second;
				}
			}
		}
	}

	return nullptr;
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
	if(root)
		root->Delete();
	gameobjects.clear();
	root = nullptr;
	selected_gameobject = nullptr;

	quad_tree->Clear();
	quad_tree->Create(AABB(float3(-160, -60, -160), float3(160, 60, 160)));
}

void ModuleScene::DeleteGameObject(GameObject* go)
{
	if (go == root)
		return;

	if (go->isStatic())
		SetGameObjectStatic(go, false);

	go->transform->GetParent()->RemoveChild(go->transform);
	RecursiveErase(go);
	go->Delete();

	go = nullptr;
}

bool ModuleScene::HasFile()
{
	return !file.empty();
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

void ModuleScene::AccelerateFrustumCulling(ComponentCamera * camera)
{
	//if the main camera has fustrum culling enabled
	if (camera->enable_frustum_culling) {
		std::vector<GameObject*> objects_hit;
		quad_tree->CollectIntersections(objects_hit, camera->frustum); //first collect static objects that intersect with the frustum

		for (std::map<uint, GameObject*>::iterator it = gameobjects.begin(); it != gameobjects.end(); ++it)
		{
			if (!it->second->isStatic()) {//then we test it with the dynamic objects
				if (it->second->HasComponent(COMPONENT_TYPE::MESH)) {
					if (camera->frustum.Intersects(it->second->aabb)) {
						objects_hit.push_back(it->second);
					}
				}
			}
		}

		for (int i = 0; i < objects_hit.size(); ++i) {
			ComponentMesh* c_mesh = (ComponentMesh*)objects_hit[i]->GetComponent(COMPONENT_TYPE::MESH);//for every object intersecting with the frustum, we set to_draw true so it will be rendered.
			c_mesh->to_draw = true;
		}
	}
	else {
		for (std::map<uint, GameObject*>::iterator it = gameobjects.begin(); it != gameobjects.end(); ++it)
		{
			if (it->second->HasComponent(COMPONENT_TYPE::MESH)) {
				ComponentMesh* c_mesh = (ComponentMesh*)it->second->GetComponent(COMPONENT_TYPE::MESH);//else we draw every object in the scene
				c_mesh->to_draw = true;
			}
		}
	}
}

void ModuleScene::SetGameObjectStatic(GameObject* go, bool state)
{
	go->SetStatic(state);
	if (state) {
			quad_tree->InsertGameObject(go);

		for (int i = 0; i < go->transform->GetChildren().size(); i++)
			App->scene->SetGameObjectStatic(go->transform->GetChildren()[i]->gameobject, true);
	}
	else {
			quad_tree->RemoveGameObject(go);

		for (int i = 0; i < go->transform->GetChildren().size(); i++)
			App->scene->SetGameObjectStatic(go->transform->GetChildren()[i]->gameobject, false);
	}
}

void ModuleScene::ResetScene()
{
	DeleteGameObjects();
	quad_tree->Clear();
	CreateDefaultScene();
}

void ModuleScene::CreateDefaultScene()
{
	selected_gameobject = nullptr;
	scene_name = DEFAULT_NAME;
	root = CreateRootGameObject();

	GameObject* obj_camera = CreateGameObject(root, "Main Camera");
	ComponentCamera* cam = (ComponentCamera*) obj_camera->AddComponent(COMPONENT_TYPE::CAMERA);
	quad_tree = new Quadtree();
	quad_tree->Create(AABB(float3(-160, -60, -160), float3(160, 60, 160)));
	cam->SetAsMainCamera(true);
	cam->gameobject->transform->SetPosition({ 0.f, 1.0f, -80.0f });

	file = "";
}