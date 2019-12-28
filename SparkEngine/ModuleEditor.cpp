#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
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
#include "PanelAnimator.h"
#include "PanelAnimation.h"
#include "MeshImporter.h"

#include "TextureMetaFile.h"

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
	panels[P_ANIMATOR] = new PanelAnimator(true);
	panels[P_ANIMATION] = new PanelAnimation(false);

	Load(it);

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

	atlas = (ResourceTexture*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_TEXTURE, App->GenerateID());
	atlas->SetExportedFile(atlas_path);
	atlas->SetFile(atlas_path);
	atlas->meta = new TextureMetaFile();
	atlas->AddReference();

	return true;
}

bool ModuleEditor::Load(const nlohmann::json::iterator & it)
{
	bool mode_state = (*it)["mode"];
	atlas_path = (*it)["atlas"].get<std::string>();
	icon_size = (*it)["icon_size"];
	((PanelProject*)panels[PROJECT])->SetItemSize((*it)["image_size"], (*it)["text_size"]);

	SetGlobalMode(mode_state);

	return true;
}

bool ModuleEditor::Save(nlohmann::json & it)
{
	uint img_size, txt_size;
	((PanelProject*)panels[PROJECT])->GetItemSize(img_size, txt_size);

	it[name] = {
		{ "mode", App->scene->global_mode },
		{ "atlas", atlas_path },
		{ "icon_size", icon_size },
		{ "image_size", img_size },
		{ "text_size", txt_size }
	};

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

std::string ModuleEditor::GetProjectPanelPath()
{
	return ((PanelProject*)panels[PROJECT])->GetCurrentPath();
}

update_status ModuleEditor::Update()
{
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();


	if(App->scene->selected_gameobject)
		HandleTransformInputs();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
			{
				App->scene->ResetScene();
			}
			if (ImGui::MenuItem("Save", "Ctrl+S"))
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
			if (ImGui::MenuItem("Animator", nullptr, panels[P_ANIMATOR]->IsActive()))
				panels[P_ANIMATOR]->Activate();
			if (ImGui::MenuItem("Animation", nullptr, panels[P_ANIMATION]->IsActive()))
				panels[P_ANIMATION]->Activate();
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
		bool accepted = false;
		if (ImGui::BeginPopupModal("Overwrite"))
		{
			ImGui::Text("A file with that name already exists. Do you want to overwrite it?");
			if (ImGui::Button("Yes", ImVec2(120, 0)))
			{
				App->scene->SetName(scene_name);
				App->scene->SaveScene();
				accepted = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(120, 0)))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)) || accepted) {

			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	open_save_popup = false;
	BeginDockSpace();

	if (ImGui::BeginMenuBar())
	{
		ImGui::PushID("Translate_Button");
		if (ImGui::ImageButton((ImTextureID)atlas->buffer_id, ImVec2(25,18), ImVec2((float)icon_size*2/atlas->width, 1),
			ImVec2(1, (float)icon_size * 2 / atlas->height), 1))
			guizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
		ImGui::PopID();

		ImGui::PushID("Rotate_Button");
		if (ImGui::ImageButton((ImTextureID)atlas->buffer_id, ImVec2(25, 18), ImVec2((float)icon_size /atlas->width, (float)icon_size * 2 / atlas->height),
			ImVec2((float)icon_size * 2 / atlas->width, (float)icon_size / atlas->height), 1))
			guizmo_operation = ImGuizmo::OPERATION::ROTATE;
		ImGui::PopID();

		ImGui::PushID("Scale_Button");
		if (ImGui::ImageButton((ImTextureID)atlas->buffer_id, ImVec2(25, 18), ImVec2(0, (float)icon_size * 2 / atlas->height),
			ImVec2((float)icon_size / atlas->width, (float)icon_size / atlas->height), 1))
			guizmo_operation = ImGuizmo::OPERATION::SCALE;
		ImGui::PopID();

		if (ImGui::Button(mode.c_str())) {
			SetGlobalMode(!App->scene->global_mode);
		}

		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.45f);
		ImGui::PushID("Play_Button");
		if (!App->IsPlay() && !App->IsPaused())
		{
			if (ImGui::ImageButton((ImTextureID)atlas->buffer_id, ImVec2(25, 18), ImVec2(0, 1),
				ImVec2((float)icon_size / atlas->width, (float)icon_size * 2 / atlas->height),1))
				App->Play();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32( 255, 0, 0, 185 ));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 0, 0, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 0, 0, 100));
			if (ImGui::ImageButton((ImTextureID)atlas->buffer_id, ImVec2(25, 18), ImVec2(0, 1),
				ImVec2((float)icon_size / atlas->width, (float)icon_size * 2 / atlas->height),1))
				App->Stop();
			ImGui::PopStyleColor(3);
		}
		ImGui::PopID();

		ImGui::PushID("Pause_Button");
		if (!App->IsPaused())
		{
			if (ImGui::ImageButton((ImTextureID)atlas->buffer_id, ImVec2(20, 14), ImVec2((float)icon_size / atlas->width, 1),
				ImVec2((float)icon_size * 2 / atlas->width, (float)icon_size * 2 / atlas->height)))
				if(App->IsPlay())
					App->Pause();
		}
		else
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			if (ImGui::ImageButton((ImTextureID)atlas->buffer_id, ImVec2(25, 18), ImVec2((float)icon_size / atlas->width, 1),
				ImVec2((float)icon_size*2 / atlas->width, (float)icon_size * 2 /atlas->height)))
				App->Resume();
			ImGui::PopStyleVar();
		}
		ImGui::PopID();

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

void ModuleEditor::SaveScene()
{
	if (App->scene->HasFile())
		App->scene->SaveScene();
	else
		open_save_popup = true;
}

void ModuleEditor::ReloadProjectWindow()
{
	((PanelProject*)panels[PROJECT])->Reload();
}

bool ModuleEditor::IsInsideSceneWindow(const float2 &pos)
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
