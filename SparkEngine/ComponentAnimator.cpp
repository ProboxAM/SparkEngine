#include "Application.h"
#include "ModuleResources.h"
#include "MathGeoLib/Math/MathAll.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "NodeEditor/Include/imgui_node_editor.h"

#include "ResourceAnimation.h"
#include "ResourceAnimatorController.h"

#include "ComponentAnimator.h"

ComponentAnimator::ComponentAnimator(GameObject * gameobject) : Component(gameobject)
{
	animator_controller = (ResourceAnimatorController*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_ANIMATOR, App->resources->GenerateNewUID());
	App->resources->GetAndReference(4190971176);
	animator_controller->AddState("default state", nullptr);
	animator_controller->AddState("default state2", nullptr);
	animator_controller->AddTransition(animator_controller->GetStates()[0], animator_controller->GetStates()[1], false, 0);
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
	animator_controller->Play(name);
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

void ComponentAnimator::OnPlay()
{
	animator_controller->Play();
}

ResourceAnimatorController * ComponentAnimator::GetResourceAnimatorController()
{
	return animator_controller;
}

bool ComponentAnimator::Save(const nlohmann::json::iterator & it)
{
	nlohmann::json object = {
		{ "active", active },
		{ "type", type },
		{ "controller",animator_controller->GetID() }
	};

	it.value().push_back(object);

	return true;
}

bool ComponentAnimator::Load(const nlohmann::json comp)
{
	active = comp["active"];
	type = comp["type"];
	animator_controller = (ResourceAnimatorController*)App->resources->GetAndReference(comp["controller"]);

	return true;
}

