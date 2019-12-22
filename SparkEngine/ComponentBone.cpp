#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ResourceBone.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBone.h"



ComponentBone::ComponentBone(GameObject* gameobject): Component(gameobject)
{
}


ComponentBone::~ComponentBone()
{
}

void ComponentBone::Draw()
{
	if (!debug_draw)
		return;

	App->renderer3D->DrawSphere(gameobject->transform->GetPosition(), 1);
}

void ComponentBone::AddBone(ResourceBone * b)
{
	if(bone)
		bone = b;

	debug_draw = true;
}
