#ifndef _MODULE_EDITOR_H_
#define _MODULE_EDITOR_H_

#include "Module.h"
#include "ImGuizmo.h"
#include <string>
#include <vector>

class Panel;
enum KEY_STATE;
class ImRect;

enum Panel_Type {
	CONFIG,
	CONSOLE,
	ABOUT,
	HIERARCHY,
	INSPECTOR,
	SCENE,
	GAME,
	PROJECT,
	RESOURCES,
	TOTAL
};

//enum IMAGE_TYPE {
//	PLAY,
//	PAUSE,
//	TRANSLATE,
//	ROTATE,
//	SCALE,
//	SCENE,
//	MODEL,
//	TOTAL
//};

class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();
	bool Init(const nlohmann::json::iterator& it);
	bool CleanUp();
	void Draw();
	bool Start();
	update_status Update();
	update_status PreUpdate();

	void LogInput(int key, KEY_STATE state, bool mouse = false);
	void LogDebug(const char* text);
	void LogFrame(float fps, float ms);
	bool IsInsideSceneWindow(float2 pos);
	std::vector<Panel*> GetPanels() {
		return panels;
	}
	Panel* GetPanel(Panel_Type type);

	void SetGlobalMode(const bool on);

	ImGuizmo::OPERATION guizmo_operation;
	ImGuizmo::MODE guizmo_mode;

	float4 button_section[TOTAL];

private:
	std::vector<Panel*> panels;
	void BeginDockSpace();
	std::string mode;

	void HandleTransformInputs();
};

#endif // !_MODULE_EDITOR_H_



