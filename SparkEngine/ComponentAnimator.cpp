#include "Application.h"
#include "ModuleResources.h"

#include "NodeEditor/Include/imgui_node_editor.h"

#include "ResourceAnimatorController.h"

#include "ComponentAnimator.h"

ComponentAnimator::ComponentAnimator(GameObject * gameobject) : Component(gameobject)
{
	animator_controller = (ResourceAnimatorController*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_ANIMATOR, App->resources->GenerateNewUID());
	animator_controller->AddState("New State", new Clip("clip01", App->resources->GenerateNewUID(), true));
}

ComponentAnimator::~ComponentAnimator()
{

}

void ComponentAnimator::Update()
{

}

ResourceAnimatorController * ComponentAnimator::GetResourceAnimatorController()
{
	return animator_controller;
}

