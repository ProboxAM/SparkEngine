#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"


#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "glew/glew.h"




ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled)
{
	name = "Editor";
}


ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init(nlohmann::json::iterator it)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	app_name = App->GetName();
	org_name = App->GetOrganization();
	refresh_rate = App->window->GetRefreshRate();
	brightness = App->window->GetBrightness();
	width = App->window->GetWindowWidth();
	height = App->window->GetWindowHeight();
	window_settings[FULLSCREEN] = App->window->GetFullscreen();
	window_settings[BORDERLESS] = App->window->GetBorderless();
	window_settings[RESIZABLE] = App->window->GetResizable();
	window_settings[FSDESKTOP] = App->window->GetFullDesktop();

	SDL_VERSION(&compiled_version);

	for (int i = 0; i < 4; i++)
	{
		window_settings[i] = false;
	}

	return true;
}

bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleEditor::Draw()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::Checkbox("About", &show_about);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (show_demo)
		ImGui::ShowDemoWindow(&show_demo);
	if (show_config)
	{
		ImGui::Begin("Configuration", &show_config);

		if (ImGui::Button("Save"))
			App->SaveSettings();

		if (ImGui::CollapsingHeader("Application"))
		{
			if (ImGui::InputText("Name", &app_name, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				App->SetName(app_name);
			if (ImGui::InputText("Organization", &org_name, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				App->SetOrganization(org_name);
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

			ImGui::Text("Vendor:");
			ImGui::SameLine();
			ImGui::TextColored({ 0, 255, 255, 255 }, (const char*)glGetString(GL_VENDOR));

			ImGui::Text("GPU Model");
			ImGui::SameLine();
			ImGui::TextColored({ 0, 255, 255, 255 }, (const char*)glGetString(GL_RENDERER));			
		}
		if (ImGui::CollapsingHeader("Renderer"))
		{
			if (ImGui::Checkbox("Depth Test", &depth_test))
				App->renderer3D->GLEnable(GL_DEPTH_TEST, depth_test);
			ImGui::SameLine();
			if (ImGui::Checkbox("Cull Face", &cull_face))
				App->renderer3D->GLEnable(GL_CULL_FACE, cull_face);
			ImGui::SameLine();
			if (ImGui::Checkbox("Lighting", &lighting))
				App->renderer3D->GLEnable(GL_LIGHTING, lighting);

			if (ImGui::Checkbox("Color Material", &color_material))
				App->renderer3D->GLEnable(GL_COLOR_MATERIAL, color_material);
			ImGui::SameLine();
			if (ImGui::Checkbox("Texture 2D", &texture_2D))
				App->renderer3D->GLEnable(GL_TEXTURE_2D, texture_2D);
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

	if (show_about) {
		if (ImGui::Begin("About", &show_about))
		{
			// Engine description
			ImGui::Text("Spark Engine");
			ImGui::Spacing();
			ImGui::TextWrapped("SparkEngine is a 3d videogame engine developed during the Game Development grade at CITM Terrasa (Spain).");
			if (ImGui::Button("SparkEngine Github")) { App->OpenWebURL("https://github.com/ProboxAM/SparkEngine"); }
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			// Authors
			ImGui::Text("Authors");
			ImGui::Spacing();
			if (ImGui::Button("Marc Guillen")) { App->OpenWebURL("https://github.com/Marcgs96"); }
			if (ImGui::Button("Axel Alavedra")) { App->OpenWebURL("https://github.com/AxelAlavedra"); }
			ImGui::Spacing();
			if (ImGui::Button("ProBoxAM")) { App->OpenWebURL("https://github.com/ProboxAM"); }
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			// Libraries
			ImGui::Text("Libraries");
			ImGui::Spacing();

			SDL_version version;
			SDL_GetVersion(&version);
			if (ImGui::Button("SDL")) { App->OpenWebURL("https://www.libsdl.org/"); }
			ImGui::SameLine(); ImGui::TextWrapped("%i.%i.%i", version.major, version.minor, version.patch);

			if (ImGui::Button("OpenGL")) { App->OpenWebURL("https://www.opengl.org/"); }
			ImGui::SameLine(); ImGui::TextWrapped("%s", glGetString(GL_VERSION));

			if (ImGui::Button("Glew")) { App->OpenWebURL("http://glew.sourceforge.net/"); }
			ImGui::SameLine(); ImGui::TextWrapped("%s", glewGetString(GLEW_VERSION));

			if (ImGui::Button("ImGui")) { App->OpenWebURL("https://github.com/ocornut/imgui"); }
			ImGui::SameLine(); ImGui::TextWrapped("%s", ImGui::GetVersion());

			if (ImGui::Button("MathGeoLib")) { App->OpenWebURL("https://github.com/juj/MathGeoLib"); }
			ImGui::SameLine(); ImGui::TextWrapped("1.5");

			if (ImGui::Button("PCG Random Number Generator")) { App->OpenWebURL("http://www.pcg-random.org/"); }
			ImGui::SameLine(); ImGui::TextWrapped("0.98");

			if (ImGui::Button("JSON for modern C++")) { App->OpenWebURL("https://github.com/nlohmann/json"); }
			ImGui::SameLine(); ImGui::TextWrapped("3.7.0");


			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			// License
			ImGui::TextWrapped("MIT License");
			ImGui::TextWrapped("Copyright(c) 2019 SparkEngine (Marc Guillen and Axel Alavedra)");
			ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the %cSoftware%c), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:", 34, 34);
			ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.");
			ImGui::TextWrapped("THE SOFTWARE IS PROVIDED %cAS IS%c, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.", 34, 34);
		}

		ImGui::End();
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

void ModuleEditor::LogDebug(const char* text)
{
	debug_buff.appendf(text);
	move_debug_scroll = true;
}

void ModuleEditor::LogFrame(float fps, float ms)
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
	if (SDL_HasSSE42())info.append("SSE42");

	return info;
}

