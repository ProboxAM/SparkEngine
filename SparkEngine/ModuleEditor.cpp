#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "Panel.h"
#include "PanelConfiguration.h"
#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelScene.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelProject.h"
#include "MeshImporter.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_stdlib.h"
#include "ImGui/imgui_internal.h"



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

bool ModuleEditor::Init(const nlohmann::json::iterator& it)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	panels[CONFIG] = new PanelConfiguration(false);
	panels[CONSOLE] = new PanelConsole(true);
	panels[ABOUT] = new PanelAbout(false);
	panels[HIERARCHY] = new PanelHierarchy(true);
	panels[INSPECTOR] = new PanelInspector(true);
	panels[SCENE] = new PanelScene(true);
	panels[PROJECT] = new PanelProject(true);

	return true;
}

bool ModuleEditor::Start()
{
	for (std::vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	{
		(*it)->Start();
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
	BeginDockSpace();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit")) { App->exit = true; }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Configuration", nullptr, panels[CONFIG]->IsActive()))
				panels[CONFIG]->Activate();
			if (ImGui::MenuItem("Console", nullptr, panels[CONSOLE]->IsActive()))
				panels[CONSOLE]->Activate();
			if (ImGui::MenuItem("Hierarchy", nullptr, panels[HIERARCHY]->IsActive()))
				panels[HIERARCHY]->Activate();
			if (ImGui::MenuItem("Inspector", nullptr, panels[INSPECTOR]->IsActive()))
				panels[INSPECTOR]->Activate();
			if (ImGui::MenuItem("Scene", nullptr, panels[SCENE]->IsActive()))
				panels[SCENE]->Activate();
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
			if (ImGui::MenuItem("About", nullptr, panels[ABOUT]->IsActive()))
				panels[ABOUT]->Activate();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	for (std::vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	{
		if ((*it)->IsActive())
			(*it)->Draw();
	}

	//End Dockspace
	ImGui::End();


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGui::EndFrame();
}

update_status ModuleEditor::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update(float dt)
{
	

	return UPDATE_CONTINUE;
}

void ModuleEditor::LogInput(int key, KEY_STATE state, bool mouse)
{
	if(panels[CONFIG] != nullptr)
		((PanelConfiguration*)panels[CONFIG])->LogInput(key, state, mouse);
}

void ModuleEditor::LogDebug(const char* text)
{
	if (panels[CONSOLE] != nullptr)
		((PanelConsole*)panels[CONSOLE])->LogDebug(text);
}

void ModuleEditor::LogFrame(float fps, float ms)
{
	if (panels[CONFIG] != nullptr)
		((PanelConfiguration*)panels[CONFIG])->LogFrame(fps, ms);
}

bool ModuleEditor::IsInsideSceneWindow(float2 pos)
{
	return ((PanelScene*)panels[SCENE])->IsInside(pos);
}

ImRect ModuleEditor::GetSceneRect()
{
	return panels[SCENE]->GetRect();
}

void ModuleEditor::BeginDockSpace()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", NULL, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}