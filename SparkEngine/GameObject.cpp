#include "GameObject.h"
#include "ComponentTransform.h"



void GameObject::Update(float dt)
{
}

Component * GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component* c;

	switch (type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		c = new ComponentTransform();
		break;
	case COMPONENT_TYPE::MESH:
		break;
	case COMPONENT_TYPE::MATERIAL:
		break;
	case COMPONENT_TYPE::LIGHT:
		break;
	default:
		break;
	}
	return c;
}

GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}
