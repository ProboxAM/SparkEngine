#include "NodeEditor/Include/imgui_node_editor.h"

#include "ResourceAnimatorController.h"

#include "ComponentAnimator.h"

ComponentAnimator::ComponentAnimator(GameObject * gameobject) : Component(gameobject)
{
	animator_controller = new ResourceAnimatorController();
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

