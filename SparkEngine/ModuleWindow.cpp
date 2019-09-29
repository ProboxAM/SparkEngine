#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	window = NULL;
	screen_surface = NULL;
	name = "Window";
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init(nlohmann::json::iterator it)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		size = (*it)["size"];
		//Create window
		width = (int)(*it)["width"] * size;
		height = (int)(*it)["height"] * size;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		if((*it)["fullscreen"] == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
			fullscreen = true;
		}

		if((*it)["resizable"] == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
			resizable = true;
		}
		
		if((*it)["borderless"] == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
			borderless = true;
		}

		if((*it)["fullDesktop"] == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
			full_desktop = true;
		}

		window = SDL_CreateWindow(App->GetName().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
		SetWindowBrightness((*it)["brightness"]);
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

int ModuleWindow::GetWindowWidth()
{
	return width;
}

int ModuleWindow::GetWindowHeight()
{
	return height;
}

void ModuleWindow::SetWindowWidth(int width)
{
	this->width = width;
	SDL_SetWindowSize(window, width, height);
}

void ModuleWindow::SetWindowHeight(int height)
{
	this->height = height;
	SDL_SetWindowSize(window, width, height);
}

int ModuleWindow::GetRefreshRate()
{
	for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i) {

		int ret = SDL_GetCurrentDisplayMode(i, &current_displaymode);

		if (ret != 0)
			// In case of error...
			SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());

		else
			// On success, return the refresh rate.
			return current_displaymode.refresh_rate;
	}
}

int ModuleWindow::GetScreenSize()
{
	return size;
}

float ModuleWindow::GetBrightness()
{
	return brightness;
}

void ModuleWindow::SetWindowBrightness(float value)
{
	brightness = value;
	SDL_SetWindowBrightness(window, value);
}

void ModuleWindow::SetScreenMode(WINDOWSETTINGS mode, bool active)
{
	switch (mode)
	{
	case FULLSCREEN: 
		if(active) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		else SDL_SetWindowFullscreen(window, 0); 
		fullscreen = active; break;

	case FSDESKTOP: 
		if (active) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		else SDL_SetWindowFullscreen(window, 0); 
		full_desktop = active; break;
	
	case RESIZABLE: 
		if(active) SDL_SetWindowResizable(window, SDL_TRUE);
		else SDL_SetWindowResizable(window, SDL_FALSE);	
		resizable = active; break;
	
	case BORDERLESS: 
		if(active) SDL_SetWindowBordered(window, SDL_FALSE);
		else SDL_SetWindowBordered(window, SDL_TRUE); 
		borderless = active; break;

	default:
		break;
	}
}

bool ModuleWindow::GetFullscreen() {
	return fullscreen;
}
bool ModuleWindow::GetBorderless() {
	return borderless;
}
bool ModuleWindow::GetResizable() {
	return resizable;
}
bool ModuleWindow::GetFullDesktop() {
	return full_desktop;
}

bool ModuleWindow::Load(nlohmann::json::iterator it)
{
	SetWindowHeight((*it)["height"]);
	SetWindowWidth((*it)["width"]);
	SetWindowBrightness((*it)["brightness"]);
	SetScreenMode(FULLSCREEN, (*it)["fullscreen"]);
	SetScreenMode(FSDESKTOP, (*it)["fullDesktop"]);
	SetScreenMode(RESIZABLE, (*it)["resizable"]);
	SetScreenMode(BORDERLESS, (*it)["borderless"]);

	return true;
}

bool ModuleWindow::Save(nlohmann::json &it)
{
	it[name] = {
		{ "width",width },
		{ "height",height },
		{ "brightness",brightness },
		{ "size",size },
		{ "fullscreen", fullscreen },
		{ "fullDesktop", full_desktop },
		{ "resizable", resizable },
		{ "borderless", borderless },
	};

	return true;
}

