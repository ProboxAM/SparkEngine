#ifndef _COMPONENT_ANIMATOR_H_
#define _COMPONENT_ANIMATOR_H_

#include "Component.h"


class ComponentAnimator :
	public Component
{
public:
	ComponentAnimator(GameObject* gameobject);
	virtual ~ComponentAnimator();

	void Update();

private:
	//AnimatorController* animator_controller;

};

#endif // !_COMPONENT_ANIMATOR_H_

