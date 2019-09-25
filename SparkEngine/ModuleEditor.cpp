#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include "ModuleEditor.h"


ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
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

		ImGui::End();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}
