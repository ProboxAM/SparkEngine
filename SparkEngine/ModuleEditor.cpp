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

	app_name = App->GetName();
	org_name = App->GetOrganization();

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
			if (ImGui::InputText("Name", &app_name, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				App->SetName(app_name);
			if (ImGui::InputText("Organization", &org_name, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				App->SetOrganization(org_name);
			if (ImGui::SliderInt("Cap FPS", &max_fps, 0, 140))
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

update_status ModuleEditor::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}
