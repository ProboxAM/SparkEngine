#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleMeshes.h"
#include "Panel.h"
#include "PanelConfiguration.h"
#include "PanelAbout.h"
#include "PanelDebug.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"




ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled)
{
	name = "Editor";
	for (int i = 0; i != TOTAL; i++)
	{
		panels.push_back(nullptr);
	}
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

	panels[CONFIG] = new PanelConfiguration(false);
	panels[DEBUG] = new PanelDebug(true);
	panels[ABOUT] = new PanelAbout(false);
	panels[HIERARCHY] = new PanelHierarchy(true);
	panels[INSPECTOR] = new PanelInspector(true);

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
			if (ImGui::MenuItem("Configuration"))
				panels[CONFIG]->Activate();
			if (ImGui::MenuItem("Debug"))
				panels[DEBUG]->Activate();
			if (ImGui::MenuItem("Hierarchy"))
				panels[HIERARCHY]->Activate();
			if (ImGui::MenuItem("Inspector"))
				panels[INSPECTOR]->Activate();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::BeginMenu("3D Object"))
			{
				if (ImGui::MenuItem("Cube"))
					App->scene->CreatePrimitiveGameObject(P_CUBE);
				if (ImGui::MenuItem("Sphere"))
					App->scene->CreatePrimitiveGameObject(P_SPHERE);
				if (ImGui::MenuItem("Cylinder"))
					App->scene->CreatePrimitiveGameObject(P_CYLINDER);
				if (ImGui::MenuItem("Plane"))
					App->scene->CreatePrimitiveGameObject(P_PLANE);
				if (ImGui::MenuItem("Cone"))
					App->scene->CreatePrimitiveGameObject(P_CONE);
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
				panels[ABOUT]->Activate();
			ImGui::EndMenu();
		}


		ImGui::Checkbox("Show Grid", &App->scene->show_grid);
		ImGui::EndMainMenuBar();
	}

	for (std::vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	{
		if ((*it)->IsActive())
			(*it)->Draw();
	}

	return UPDATE_CONTINUE;
}

void ModuleEditor::LogInput(int key, KEY_STATE state, bool mouse)
{
	if(panels[CONFIG] != nullptr)
		((PanelConfiguration*)panels[CONFIG])->LogInput(key, state, mouse);
}

void ModuleEditor::LogDebug(const char* text)
{
	if (panels[DEBUG] != nullptr)
		((PanelDebug*)panels[DEBUG])->LogDebug(text);
}

void ModuleEditor::LogFrame(float fps, float ms)
{
	if (panels[CONFIG] != nullptr)
		((PanelConfiguration*)panels[CONFIG])->LogFrame(fps, ms);
}