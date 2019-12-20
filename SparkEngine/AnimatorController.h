#ifndef _ANIMATOR_CONTROLLER_H_
#define _ANIMATOR_CONTROLLER_H_

#include "Globals.h"

#include <vector>

class AnimatorController
{

	struct State {
		//ResourceAnimation
		uint id = 0;
		float speed = 0.f;

	};

	std::vector<State*> states;
	
public:
	AnimatorController();
	~AnimatorController();

	void CreateAnimationClip();
};

#endif // !__ANIMATOR_CONTROLLER_H_
