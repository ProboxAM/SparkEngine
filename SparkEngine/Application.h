#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "pcg_random.hpp"

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleEditor;
class ModuleImporter;
class ModuleFileSystem;
class ModuleScene;
class ModuleTextures;
class ModuleMeshes;
class ModuleResources;

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
	ModuleScene* scene;
	ModuleTextures* textures;
	ModuleMeshes* meshes;
	ModuleResources* resources;

private:
	std::string		app_name;
	std::string		organization;

	pcg32 random;

	Timer	ms_timer;
	float	dt;
	int		fps_cap = 0;
	int		fps = 0;
	int		capped_ms = 0;
	int		last_frame_ms = 0;
	std::list<Module*> list_modules;

public:

	bool exit = false;

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
	uint GenerateID();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;

#endif