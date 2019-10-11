#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject* my_go)
{
	this->my_go = my_go;
}

Component::~Component()
{
}

void Component::Update()
{
}
