#include "ResourceAnimatorController.h"



ResourceAnimatorController::ResourceAnimatorController()
{
	ed_context = ax::NodeEditor::CreateEditor();
}


ResourceAnimatorController::~ResourceAnimatorController()
{
	ax::NodeEditor::DestroyEditor(ed_context);
}

void ResourceAnimatorController::CreateAnimationClip()
{

}

ax::NodeEditor::EditorContext * ResourceAnimatorController::GetEditorContext()
{
	return ed_context;
}

std::string ResourceAnimatorController::GetName()
{
	return name;
}
