#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleScene.h"



ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
	name = "Scene";
}


ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Start()
{
	return false;
}

update_status ModuleScene::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	return false;
}

GameObject * ModuleScene::CreateGameObject()
{
	return nullptr;
}
