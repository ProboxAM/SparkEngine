#pragma once
#include "Module.h"
class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();
	bool Init();
	update_status PostUpdate(float dt);
	update_status Update(float dt);

private:
	bool demo = false;
};

