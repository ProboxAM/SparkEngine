#ifndef _COMPONENT_ANIMATOR_H_
#define _COMPONENT_ANIMATOR_H_

#include "Component.h"

class ResourceAnimatorController;


class ComponentAnimator :
	public Component
{
public:
	ComponentAnimator(GameObject* gameobject);
	virtual ~ComponentAnimator();

	void Update();

	ResourceAnimatorController* GetResourceAnimatorController();

private:
	ResourceAnimatorController* animator_controller;

};

#endif // !_COMPONENT_ANIMATOR_H_

