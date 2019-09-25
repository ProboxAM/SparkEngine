#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"


#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"




ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled)
{
	for (int i = 0; i < 4; i++)
	{
		window_settings[i] = false;
	}
}


ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	refresh_rate = App->window->GetRefreshRate();
	brightness = App->window->GetBrightness();
	SDL_VERSION(&compiled_version);

	return true;
}

bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

update_status ModuleEditor::Update(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit")) { return UPDATE_STOP; }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::Checkbox("Configuration", &show_config);
			ImGui::Checkbox("Debug", &show_debug);
			ImGui::Checkbox("Demo", &show_demo);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (show_demo)
		ImGui::ShowDemoWindow(&show_demo);
	if (show_config)
	{
		ImGui::Begin("Configuration", &show_config);
		if (ImGui::CollapsingHeader("Application"))
		{
			ImGui::InputText("Name", &app_name);
		}
		
		if (ImGui::CollapsingHeader("Window"))
		{
			if (ImGui::SliderInt("Width", &width, 0, 1920))	App->window->SetWindowWidth(width);
			if (ImGui::SliderInt("Height", &height, 0, 1080)) App->window->SetWindowHeight(height);
			if (ImGui::SliderFloat("Brightness", &brightness, 0, 1)) App->window->SetWindowBrightness(brightness);

			ImGui::Text("Refresh rate");
			ImGui::SameLine();
			std::string temp = std::to_string(refresh_rate);
			ImGui::TextColored({ 0, 255, 255, 255 }, temp.c_str());

			if (ImGui::Checkbox("Fullscreen", &window_settings[FULLSCREEN])) {
				App->window->SetScreenMode(FULLSCREEN, window_settings[FULLSCREEN]);
			}
	
			ImGui::SameLine();
			
			if (ImGui::Checkbox("Resizable", &window_settings[RESIZABLE])) {
				App->window->SetScreenMode(RESIZABLE, window_settings[RESIZABLE]);
			}
			if (ImGui::Checkbox("Borderless", &window_settings[BORDERLESS])) {
				App->window->SetScreenMode(BORDERLESS, window_settings[BORDERLESS]);
			}
		
			ImGui::SameLine();
		
			if (ImGui::Checkbox("Full Desktop", &window_settings[FSDESKTOP])) {
				App->window->SetScreenMode(FSDESKTOP, window_settings[FSDESKTOP]);
			}
		}

		if (ImGui::CollapsingHeader("Input"))
		{
			ImGui::Text("Mouse position:");
			ImGui::SameLine();
			std::string temp_string = "(" + std::to_string(App->input->GetMouseX()) + ", " + std::to_string(App->input->GetMouseY()) + ")";
			ImGui::TextColored({ 0,255,255,255 }, temp_string.c_str());

			ImGui::Text("Mouse motion:");
			ImGui::SameLine();
			temp_string = "(" + std::to_string(App->input->GetMouseXMotion()) + ", " + std::to_string(App->input->GetMouseYMotion()) + ")";
			ImGui::TextColored({ 0,255,255,255 }, temp_string.c_str());

			ImGui::Text("Mouse wheel:");
			ImGui::SameLine();
			temp_string = std::to_string(App->input->GetMouseZ());
			ImGui::TextColored({ 0,255,255,255 }, temp_string.c_str());
			ImGui::Separator();

			ImGui::BeginChild("Log");
			ImGui::TextUnformatted(input_buff.begin());
			if (move_input_scroll)
			{
				ImGui::SetScrollHere(1.0f);
				move_input_scroll = false;
			}
			ImGui::EndChild();
		}

		if (ImGui::CollapsingHeader("Hardware"))
		{
			ImGui::Text("SDL Version:");
			ImGui::SameLine();
			std::string temp = std::to_string(compiled_version.major) + "." + std::to_string(compiled_version.minor) + "." + std::to_string(compiled_version.patch);
			ImGui::TextColored({ 0, 255, 255, 255 }, temp.c_str());

			ImGui::Separator();

			ImGui::Text("CPU cores:");
			ImGui::SameLine();
			temp = std::to_string(SDL_GetCPUCount()) + " (Cache: " + std::to_string(SDL_GetCPUCacheLineSize()) + "Kb)";
			ImGui::TextColored({ 0, 255, 255, 255 }, temp.c_str());

			ImGui::Text("System RAM:");
			ImGui::SameLine();
			temp = std::to_string(SDL_GetSystemRAM()) + "Mb";
			ImGui::TextColored({ 0, 255, 255, 255 }, temp.c_str());			
			
			ImGui::Text("Caps:");
			ImGui::SameLine();
			ImGui::TextColored({ 0, 255, 255, 255 }, GetCpuInfo().c_str());

			ImGui::Separator();
			
		}

		ImGui::End();
	}
	if (show_debug)
	{
		ImGui::BeginChild("Log");
		ImGui::TextUnformatted(debug_buff.begin());
		if (move_debug_scroll)
		{
			ImGui::SetScrollHere(1.0f);
			move_debug_scroll = false;
		}
		ImGui::EndChild();
	}

	return UPDATE_CONTINUE;
}

void ModuleEditor::LogInput(int key, KEY_STATE state, bool mouse)
{
	std::string temp_string;
	std::string states[] = { "IDLE","DOWN","REPEAT","UP" };

	if (mouse)
		temp_string = "Mouse: " + std::to_string(key) + " " + states[state] + "\n";	
	else
		temp_string = "Keyboard: " + std::to_string(key) + " " + states[state] + "\n";

	input_buff.appendf(temp_string.c_str());
	move_input_scroll = true;
}

void ModuleEditor::Log(const char* text)
{
	debug_buff.appendf(text);
	move_debug_scroll = true;
}

std::string ModuleEditor::GetCpuInfo()
{

	std::string info;

	if (SDL_Has3DNow())info.append("3DNow, ");
	if (SDL_HasAVX())info.append("AVX, ");
	if (SDL_HasAVX2())info.append("AVX2, ");
	if (SDL_HasMMX())info.append("MMX, ");
	if (SDL_HasRDTSC())info.append("RDTSC, ");
	if (SDL_HasSSE())info.append("SSE, ");
	if (SDL_HasSSE2())info.append("SSE2, ");
	if (SDL_HasSSE3())info.append("SSE3, ");
	if (SDL_HasSSE41())info.append("SSE41, ");
	if (SDL_HasSSE42())info.append("SSE42, ");

	return info;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}
