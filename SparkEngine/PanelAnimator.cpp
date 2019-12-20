#include "Application.h"
#include "ModuleWindow.h"

#include "PanelAnimator.h"

#define CHECKBOX_SIZE 50

PanelAnimator::PanelAnimator(bool active):Panel(active)
{
}

PanelAnimator::~PanelAnimator()
{

}


void PanelAnimator::Draw()
{
	ImGui::Begin("Animator", &active);
	ImGui::End();
}

void PanelAnimator::Start()
{

}

bool PanelAnimator::IsInside(const float2 & pos) const
{
	return false;
}