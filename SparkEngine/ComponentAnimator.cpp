#include "Application.h"
#include "ModuleResources.h"
#include "MathGeoLib/Math/MathAll.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "NodeEditor/Include/imgui_node_editor.h"

#include "ResourceAnimatorController.h"

#include "ComponentAnimator.h"

ComponentAnimator::ComponentAnimator(GameObject * gameobject) : Component(gameobject)
{
	animator_controller = (ResourceAnimatorController*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_ANIMATOR, App->resources->GenerateNewUID());
	App->resources->GetAndReference(3560868769);
	animator_controller->AddClip("asd", 3560868769, true);
	animator_controller->PlayClip(animator_controller->FindClip("asd").GetName(), animator_controller->FindClip("asd").GetResource(), animator_controller->FindClip("asd").GetLoop());
}

ComponentAnimator::~ComponentAnimator()
{
	delete animator_controller;
}

void ComponentAnimator::Update()
{
	animator_controller->Update();

	if (gameobject && App->IsPlay())
		UpdateAnimation(gameobject);
}

void ComponentAnimator::PlayState(std::string name)
{
	State state_to_play = animator_controller->FindState(name);
	animator_controller->PlayClip(state_to_play.GetClip()->GetName(), state_to_play.GetClip()->GetResource(), state_to_play.GetClip()->GetLoop());
}

void ComponentAnimator::UpdateAnimation(GameObject * go_to_update)
{
	float3 position, scale;
	Quat rotation;

	if (animator_controller->GetTransform(go_to_update->GetName().c_str(), position, rotation, scale))
	{
		go_to_update->transform->SetLocalPosition(position);
		go_to_update->transform->SetLocalRotation(rotation);
		go_to_update->transform->SetLocalScale(scale);
	}

	for (int i = 0; i < go_to_update->transform->GetChildren().size(); i++)
	{
		UpdateAnimation(go_to_update->transform->GetChildren()[i]->gameobject);
	}
}

ResourceAnimatorController * ComponentAnimator::GetResourceAnimatorController()
{
	return animator_controller;
}

