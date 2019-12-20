#include "ComponentAnimator.h"
#include "NodeEditor/Include/imgui_node_editor.h"

namespace ed = ax::NodeEditor;

static ed::EditorContext* g_Context = nullptr;

ComponentAnimator::ComponentAnimator(GameObject * gameobject) : Component(gameobject)
{
	g_Context = ed::CreateEditor();
}

ComponentAnimator::~ComponentAnimator()
{
	ed::DestroyEditor(g_Context);
}

void ComponentAnimator::Update()
{
	ed::SetCurrentEditor(g_Context);

	ed::Begin("My Editor");

	int uniqueId = 1;

	// Start drawing nodes.
	ed::BeginNode(uniqueId++);
	ImGui::Text("Node A");
	ed::BeginPin(uniqueId++, ed::PinKind::Input);
	ImGui::Text("-> In");
	ed::EndPin();
	ImGui::SameLine();
	ed::BeginPin(uniqueId++, ed::PinKind::Output);
	ImGui::Text("Out ->");
	ed::EndPin();
	ed::EndNode();

	ed::End();
}

