#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "Panel.h"
#include "PanelConfiguration.h"
#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelScene.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelProject.h"
#include "PanelResources.h"
#include "PanelGame.h"
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
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	panels[CONFIG] = new PanelConfiguration(false);
	panels[CONSOLE] = new PanelConsole(true);
	panels[ABOUT] = new PanelAbout(false);
	panels[HIERARCHY] = new PanelHierarchy(true);
	panels[INSPECTOR] = new PanelInspector(true);
	panels[SCENE] = new PanelScene(true);
	panels[GAME] = new PanelGame(true);
	panels[PROJECT] = new PanelProject(true);
	panels[RESOURCES] = new PanelResources(true);

	mode = "Local";

	return true;
}

bool ModuleEditor::Start()
{
	for (std::vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	{
		(*it)->Start();
	}

	guizmo_mode = ImGuizmo::MODE::LOCAL;
	guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;

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

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	ImGui::NewFrame();

	bool open_save_popup = false;

	if(App->scene->selected_gameobject)
		HandleTransformInputs();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				App->scene->ResetScene();
			}
			if (ImGui::MenuItem("Save")) 
			{ 
				if (App->scene->HasFile())
					App->scene->SaveScene();
				else
					open_save_popup = true;
			}
			if (ImGui::MenuItem("Save as...")) {
				open_save_popup = true;
			}
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
			if (ImGui::MenuItem("Game", nullptr, panels[GAME]->IsActive()))
				panels[GAME]->Activate();
			if (ImGui::MenuItem("Project", nullptr, panels[PROJECT]->IsActive()))
				panels[PROJECT]->Activate();
			if (ImGui::MenuItem("Resources", nullptr, panels[RESOURCES]->IsActive()))
				panels[RESOURCES]->Activate();
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

			if (ImGui::Selectable("Create Empty")) {
				App->scene->CreateGameObject(App->scene->root);
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
	//Save scene popup
	static std::string scene_name;
	if (open_save_popup)
	{
		ImGui::OpenPopup("Save as");
		scene_name = App->scene->GetName();
	}		
	if (ImGui::BeginPopupModal("Save as"))
	{
		ImGui::InputText("Name", &scene_name, ImGuiInputTextFlags_CharsNoBlank);

		if (ImGui::Button("Save", ImVec2(120, 0))) {

			std::string scene_file = ASSETS_FOLDER + scene_name + SCENE_EXTENSION;
			bool exists = App->fsystem->Exists(scene_file.c_str());
			if (exists)
				ImGui::OpenPopup("Overwrite");
			if (!exists)
			{
				App->scene->SetName(scene_name);
				App->scene->SaveScene();
				ImGui::CloseCurrentPopup();
			}			
		}
		if (ImGui::BeginPopupModal("Overwrite"))
		{
			ImGui::Text("A file with that name already exists. Do you want to overwrite it?");
			if (ImGui::Button("Yes", ImVec2(120, 0)))
			{
				App->scene->SetName(scene_name);
				App->scene->SaveScene();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(120, 0)))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	BeginDockSpace();

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::Button("Translate")) {
			guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
		}
		if(ImGui::Button("Rotate")) {
			guizmo_operation = ImGuizmo::OPERATION::ROTATE;
		}
		if (ImGui::Button("Scale")) {
			guizmo_operation = ImGuizmo::OPERATION::SCALE;
		}
		if (ImGui::Button(mode.c_str())) {
			SetGlobalMode(!App->scene->global_mode);
		}

		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.4f);
		if (!App->IsPlay() && !App->IsPaused())
		{
			if (ImGui::Button("Play"))
				App->Play();
		}
		else
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			if (ImGui::Button("Play"))
				App->Stop();
			ImGui::PopStyleVar();
		}

		if (!App->IsPaused())
		{
			if (ImGui::Button("Pause"))
				if(App->IsPlay())
					App->Pause();
		}
		else
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			if (ImGui::Button("Pause"))
				App->Resume();
			ImGui::PopStyleVar();
		}

		ImGui::EndMenuBar();
	}

	for (std::vector<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
	{
		if ((*it)->IsActive())
			(*it)->Draw();
	}

	//End Dockspace
	ImGui::End();
	ImGui::EndFrame();
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

Panel * ModuleEditor::GetPanel(Panel_Type type)
{
	return panels[type];
}

void ModuleEditor::SetGlobalMode(const bool on)
{
	if (on) {
		mode = "World";
		guizmo_mode = ImGuizmo::MODE::WORLD;
		App->scene->global_mode = true;
	}
	else {
		mode = "Local";
		guizmo_mode = ImGuizmo::MODE::LOCAL;
		App->scene->global_mode = false;
	}
}

void ModuleEditor::BeginDockSpace()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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


void ModuleEditor::HandleTransformInputs()
{
	if (!ImGuizmo::IsUsing())
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			guizmo_operation = ImGuizmo::OPERATION::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			guizmo_operation = ImGuizmo::OPERATION::SCALE;
	}
}
