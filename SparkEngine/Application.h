#pragma once

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleImporter.h"
#include "ModuleFileSystem.h"


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleImporter* importer;
	ModuleFileSystem* fsystem;

private:
	std::string		app_name;
	std::string		organization;

	Timer	ms_timer;
	Timer	fps_timer;
	float	dt;
	int		fps_cap = 0;
	int		fps = 0;
	int		fps_count = 0;
	int		capped_ms = 0;
	int		last_frame_ms = 0;
	std::list<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void SetName(std::string name);
	void SetOrganization(std::string org);
	void SetFPSCap(int cap);
	void SaveSettings();
	void LoadSettings();

	std::string GetName();
	std::string GetOrganization();
	int GetFPSCap();

	void OpenWebURL(std::string path);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;