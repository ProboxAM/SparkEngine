#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
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
		//Create window
		width = SCREEN_WIDTH * SCREEN_SIZE;
		height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}
		
		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
		App->SetName(TITLE);

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

		brightness = SDL_GetWindowBrightness(window);
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

float ModuleWindow::GetBrightness()
{
	return brightness;
}

void ModuleWindow::SetWindowBrightness(float value)
{
	SDL_SetWindowBrightness(window, value);
}

void ModuleWindow::SetScreenMode(WINDOWSETTINGS mode, bool active)
{
	switch (mode)
	{
	case FULLSCREEN: 
		if(active) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		else SDL_SetWindowFullscreen(window, 0); break;

	case FSDESKTOP: 
		if (active) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		else SDL_SetWindowFullscreen(window, 0); break;
	
	case RESIZABLE: 
		if(active) SDL_SetWindowResizable(window, SDL_TRUE);
		else SDL_SetWindowResizable(window, SDL_FALSE);	break;
	
	case BORDERLESS: 
		if(active) SDL_SetWindowBordered(window, SDL_FALSE);
		else SDL_SetWindowBordered(window, SDL_TRUE); break;

	default:
		break;
	}
}

