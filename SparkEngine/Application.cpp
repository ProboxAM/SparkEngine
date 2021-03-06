#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleImporter.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "Application.h"
#include "nlohmann\json.hpp"
#include <random>
#include <fstream>
#include <iomanip>

Application::Application()
{
	window = new ModuleWindow(true);
	time = new ModuleTime(true);
	input = new ModuleInput(true);
	renderer3D = new ModuleRenderer3D(true);
	camera = new ModuleCamera3D(true);
	editor = new ModuleEditor(true);
	importer = new ModuleImporter(true);
	fsystem = new ModuleFileSystem(ASSETS_FOLDER);
	scene = new ModuleScene(true);
	resources = new ModuleResources(true);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(time);
	AddModule(resources);
	AddModule(editor);
	AddModule(camera);
	AddModule(input);
	AddModule(fsystem);
	AddModule(importer);
	AddModule(scene);
	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	while (item != list_modules.rend())
	{
		delete (*item);
		++item;
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	pcg_extras::seed_seq_from<std::random_device> seed_source;
	pcg32 rng(seed_source);
	random = rng;

	std::ifstream i("Settings/Config.json");
	nlohmann::json j = nlohmann::json::parse(i);

	SetName(j["Application"]["Title"]);
	SetOrganization(j["Application"]["Organization"]);

	// Call Init() in all modules
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++)
	{
		ret = (*it)->Init(j.find((*it)->name));
	}

	if (ret)
	{
		for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++)
		{
			ret = (*it)->Start();
		}
	}
	ms_timer.Start();

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();

	switch (state)
	{
		case ENGINE_WANTS_PLAY:
			LOG("Engine state: PLAY");
			time->StartGameClock();
			App->scene->OnPlay();
			state = ENGINE_PLAY;
			break;
		case ENGINE_WANTS_PAUSE:
			LOG("Engine state: PAUSE");
			time->PauseGameClock();
			state = ENGINE_PAUSE;
			break;
		case ENGINE_WANTS_EDITOR:
			LOG("Engine state: EDITOR");
			time->StopGameClock();
			App->scene->OnStop();
			state = ENGINE_EDITOR;
			break;
		default:
			break;
	}

	time->PrepareUpdate(dt);
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	last_frame_ms = ms_timer.Read();

	if (capped_ms > 0 && (last_frame_ms < capped_ms))
	{
		SDL_Delay(capped_ms - last_frame_ms);
		last_frame_ms += capped_ms - last_frame_ms;
	}
	fps = 1000.0 / last_frame_ms;

	App->editor->LogFrame((float)fps, (float)last_frame_ms);
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == true; it++)
	{
		ret = (*it)->PreUpdate();
	}

	if (ret)
	{
		for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == true; it++)
		{
			ret = (*it)->Update();
		}
	}

	if (ret)
	{
		for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == true; it++)
		{
			ret = (*it)->PostUpdate();
		}
	}

	if (exit)
		ret = UPDATE_STOP;

	FinishUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	SaveSettings();

	while (item != list_modules.rend() && ret == true)
	{
		ret = (*item)->CleanUp();
		++item;
	}

	return ret;
}

void Application::SetName(std::string name)
{
	app_name = name;
	App->window->SetTitle(app_name.c_str());
}

void Application::SetOrganization(std::string org)
{
	organization = org;
}

void Application::SetFPSCap(int cap)
{
	fps_cap = cap;
	if (fps_cap > 0)
		capped_ms = 1000 / fps_cap;
	else
		capped_ms = 0;
}

std::string Application::GetName()
{
	return app_name;
}

std::string Application::GetOrganization()
{
	return organization;
}

int Application::GetFPSCap()
{
	return fps_cap;
}

int Application::GetFPS()
{
	return fps;
}

float Application::GetFrameMS()
{
	return last_frame_ms;
}

void Application::OpenWebURL(std::string path)
{
	ShellExecute(0, 0, path.c_str(), 0, 0, SW_SHOW);
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::SaveSettings()
{
	nlohmann::json j;

	j["Application"] = {
		{"Title", app_name},
		{"Organization", organization}
	};

	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++)
	{
		(*it)->Save(j);
	}

	std::ofstream o("Settings/Config.json");
	o << std::setw(4) << j << std::endl;
}

void Application::LoadSettings()
{
	std::ifstream i("Settings/Config.json");
	nlohmann::json j = nlohmann::json::parse(i);

	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++)
	{
		(*it)->Load(j.find((*it)->name));
	}
}

uint Application::GenerateID()
{
	uint n = random();
	return n;
}

bool Application::IsPaused()
{
	return state == ENGINE_PAUSE;
}

bool Application::IsPlay()
{
	return state == ENGINE_PLAY;
}

bool Application::IsEditor()
{
	return state == ENGINE_EDITOR;
}

void Application::Play()
{
	state = ENGINE_WANTS_PLAY;
}

void Application::Pause()
{
	state = ENGINE_WANTS_PAUSE;
}

void Application::Stop()
{
	state = ENGINE_WANTS_EDITOR;
}

void Application::Resume()
{
	state = ENGINE_PLAY;
	time->StartGameClock();
}
