#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* gameobject)
{
	this->gameobject = gameobject;
}

Component::~Component()
{
}

void Component::Update()
{
}
