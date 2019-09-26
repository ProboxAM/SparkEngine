#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;
enum WINDOWSETTINGS { FULLSCREEN, FSDESKTOP, RESIZABLE, BORDERLESS};

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(nlohmann::json::iterator it);
	bool CleanUp();

	void SetTitle(const char* title);

	int GetWindowWidth();
	int GetWindowHeight();

	void SetWindowWidth(int width);
	void SetWindowHeight(int height);

	int GetRefreshRate();
	float GetBrightness();
	int GetScreenSize();
	
	void SetWindowBrightness(float value);
	void SetScreenMode(WINDOWSETTINGS mode, bool active);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

private:
	int width, height = 0;
	int refresh_rate = 0;
	int size = 0;
	float brightness = .0f;


	SDL_DisplayMode current_displaymode;
};

#endif // __ModuleWindow_H__