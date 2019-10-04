#ifndef _PANEL_H_
#define _PANEL_H_

#include "imgui.h"

class Panel
{
public:
	Panel(bool a) {
		active = a;
	}
	~Panel() {};

	void Activate()
	{
		active = !active;
	}

	bool IsActive() {
		return active;
	}

	virtual void Draw() {};

private:
	bool active = false;
};
#endif
