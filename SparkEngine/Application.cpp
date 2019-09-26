#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);


	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(editor);
	AddModule(input);
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

	// Call Init() in all modules
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++)
	{
		ret = (*it)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");

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
		ret = (*it)->PreUpdate(dt);
	}

	if (ret)
	{
		for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == true; it++)
		{
			ret = (*it)->Update(dt);
		}
	}

	if (ret)
	{
		for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == true; it++)
		{
			ret = (*it)->PostUpdate(dt);
		}
	}

	FinishUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

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

void Application::OpenWebURL(std::string path)
{
	ShellExecute(0, 0, path.c_str(), 0, 0, SW_SHOW);
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}