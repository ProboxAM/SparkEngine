#ifndef _PANEL_H_
#define _PANEL_H_

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

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

	virtual void Start()
	{

	}

	bool IsActive() {
		return active;
	}

	virtual void Draw() {};

protected:
	bool active = false;
};
#endif
