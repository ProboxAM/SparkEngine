#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "glew/glew.h"
#include "PanelConfiguration.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"

#define MAX_LOG 50

PanelConfiguration::PanelConfiguration(bool a) : Panel(a)
{
}


PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Start()
{
	SDL_VERSION(&compiled_version);
}

void PanelConfiguration::Draw()
{
	ImGui::Begin("Configuration");

	if (ImGui::Button("Save"))
		App->SaveSettings();
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		App->LoadSettings();

	DrawApplication();
	DrawWindow();
	DrawInput();
	DrawHardware();
	DrawRenderer();
	DrawCamera();

	ImGui::End();
}

void PanelConfiguration::LogFrame(float fps, float ms)
{
	if (fps_log.size() == MAX_LOG)
	{
		for (uint i = 1; i < MAX_LOG; ++i)
		{
			fps_log[i] = fps_log[i - 1];
			ms_log[i] = ms_log[i - 1];
		}
		fps_log[0] = fps;
		ms_log[0] = ms;
	}
	else
	{
		fps_log.insert(fps_log.begin(), fps);
		ms_log.insert(ms_log.begin(), ms);
	}
}

void PanelConfiguration::DrawApplication()
{
	if (ImGui::CollapsingHeader("Application"))
	{
		std::string app_name = App->GetName();
		if (ImGui::InputText("Name", &app_name, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			App->SetName(app_name);
		std::string org_name = App->GetOrganization();
		if (ImGui::InputText("Organization", &org_name, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			App->SetOrganization(org_name);
		int max_fps = App->GetFPSCap();
		if (ImGui::SliderInt("Cap FPS", &max_fps, 0, 144))
			App->SetFPSCap(max_fps);

		ImGui::Text("Framerate Cap:");
		ImGui::SameLine();
		ImGui::TextColored({ 0,255,255,255 }, std::to_string(App->GetFPSCap()).c_str());

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}
}

void PanelConfiguration::DrawWindow()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		int width = App->window->GetWindowWidth();
		int height = App->window->GetWindowHeight();
		float brightness = App->window->GetBrightness();
		if (ImGui::SliderInt("Width", &width, 0, 1920))	App->window->SetWindowWidth(width);
		if (ImGui::SliderInt("Height", &height, 0, 1080)) App->window->SetWindowHeight(height);
		if (ImGui::SliderFloat("Brightness", &brightness, 0, 1)) App->window->SetWindowBrightness(brightness);

		int refresh_rate = App->window->GetRefreshRate();
		ImGui::Text("Refresh rate");
		ImGui::SameLine();
		ImGui::TextColored({ 0, 255, 255, 255 }, std::to_string(refresh_rate).c_str());

		bool fullscreen = App->window->GetFullscreen();
		if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
			App->window->SetScreenMode(FULLSCREEN, fullscreen);
		}

		ImGui::SameLine();
		bool resizable = App->window->GetResizable();
		if (ImGui::Checkbox("Resizable", &resizable)) {
			App->window->SetScreenMode(RESIZABLE, resizable);
		}
		bool borderless = App->window->GetBorderless();
		if (ImGui::Checkbox("Borderless", &borderless)) {
			App->window->SetScreenMode(BORDERLESS, borderless);
		}

		ImGui::SameLine();
		bool fulldesktop = App->window->GetFullDesktop();
		if (ImGui::Checkbox("Full Desktop", &fulldesktop)) {
			App->window->SetScreenMode(FSDESKTOP, fulldesktop);
		}
	}
}

void PanelConfiguration::DrawInput()
{
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
}

void PanelConfiguration::DrawHardware()
{
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

		ImGui::Text("VRAM Budget:");
		ImGui::SameLine();
		int aux = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &aux);
		ImGui::TextColored({ 255, 255, 0, 255 }, "%iMb", aux / 1024);

		ImGui::Text("VRAM Usage:");
		ImGui::SameLine();
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &aux);
		ImGui::TextColored({ 255, 255, 0, 255 }, "%iMb", aux / 1024);

		ImGui::Text("VRAM Available:");
		ImGui::SameLine();
		glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &aux);
		ImGui::TextColored({ 255, 255, 0, 255 }, "%iMb", aux / 1024);

		ImGui::Text("VRAM Reserved:");
		ImGui::SameLine();
		glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &aux);
		ImGui::TextColored({ 255, 255, 0, 255 }, "%iMb", aux / 1024);
	}
}

void PanelConfiguration::DrawRenderer()
{
	if (ImGui::CollapsingHeader("Renderer"))
	{
		bool vsync = App->renderer3D->GetVsync();
		if (ImGui::Checkbox("Vsync", &vsync))
			App->renderer3D->SetVsync(vsync);
		bool depth_test = App->renderer3D->IsEnabled(GL_DEPTH_TEST);
		if (ImGui::Checkbox("Depth Test", &depth_test))
			App->renderer3D->GLEnable(GL_DEPTH_TEST, depth_test);
		ImGui::SameLine();
		bool cull_face = App->renderer3D->IsEnabled(GL_CULL_FACE);
		if (ImGui::Checkbox("Cull Face", &cull_face))
			App->renderer3D->GLEnable(GL_CULL_FACE, cull_face);
		ImGui::SameLine();
		bool lighting = App->renderer3D->IsEnabled(GL_LIGHTING);
		if (ImGui::Checkbox("Lighting", &lighting))
			App->renderer3D->GLEnable(GL_LIGHTING, lighting);
		bool color_material = App->renderer3D->IsEnabled(GL_COLOR_MATERIAL);
		if (ImGui::Checkbox("Color Material", &color_material))
			App->renderer3D->GLEnable(GL_COLOR_MATERIAL, color_material);
		ImGui::SameLine();
		bool texture_2D = App->renderer3D->IsEnabled(GL_TEXTURE_2D);
		if (ImGui::Checkbox("Texture 2D", &texture_2D))
			App->renderer3D->GLEnable(GL_TEXTURE_2D, texture_2D);
	}
}

void PanelConfiguration::DrawCamera()
{
	if (ImGui::CollapsingHeader("Camera3D"))
	{
		ImGui::Checkbox("Camera Inputs", &App->camera->camera_inputs_active);
		ImGui::SliderFloat("Movement Speed", &App->camera->movement_speed, 0, 100);
		ImGui::SliderFloat("Sprint Speed", &App->camera->sprint_speed, 0, 100);
	}
}

void PanelConfiguration::LogInput(int key, KEY_STATE state, bool mouse)
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

std::string PanelConfiguration::GetCpuInfo()
{
	std::string info;

	if (SDL_Has3DNow())info.append("3DNow | ");
	if (SDL_HasAVX())info.append("AVX | ");
	if (SDL_HasAVX2())info.append("AVX2 | ");
	if (SDL_HasMMX())info.append("MMX | ");
	if (SDL_HasRDTSC())info.append("RDTSC | ");
	if (SDL_HasSSE())info.append("SSE | ");
	if (SDL_HasSSE2())info.append("SSE2 | ");
	if (SDL_HasSSE3())info.append("SSE3 | ");
	if (SDL_HasSSE41())info.append("SSE41 | ");
	if (SDL_HasSSE42())info.append("SSE42 | ");

	return info;
}