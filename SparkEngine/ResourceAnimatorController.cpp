#include "ResourceAnimatorController.h"


ResourceAnimatorController::ResourceAnimatorController(uint id) : Resource(id, Resource::RESOURCE_TYPE::R_ANIMATOR)
{
	ed_context = ax::NodeEditor::CreateEditor();
}

ResourceAnimatorController::~ResourceAnimatorController()
{
	ax::NodeEditor::DestroyEditor(ed_context);
}

void ResourceAnimatorController::AddClip(std::string name, uint id, bool loop)
{
	Clip new_clip(name, id, loop);
	clips.push_back(new_clip);
}

void ResourceAnimatorController::RemoveClip(std::string name)
{
	for (std::vector<State>::iterator it = states.begin(); it != states.end(); ++it) {
		if (it->GetClip()->GetName() == name) {
			it = states.erase(it);
		}
	}

	for (std::vector<Clip>::iterator it = clips.begin(); it != clips.end(); ++it) {
		if (it->GetName() == name) {
			it = clips.erase(it);
		}
	}
}

void ResourceAnimatorController::AddState(std::string name, Clip * clip)
{
	State new_state(name, clip);
	states.push_back(new_state);
}

void ResourceAnimatorController::RemoveState(std::string name)
{
	for (std::vector<State>::iterator it = states.begin(); it != states.end(); ++it) {
		if (it->GetName() == name) {
			it = states.erase(it);
		}
	}
}

void ResourceAnimatorController::AddTransition(State * source, State * target, bool trigger, uint blend)
{
	Transition new_transition(source, target, trigger, blend);

	transitions.push_back(new_transition);
}

void ResourceAnimatorController::RemoveTransition(std::string source_name, std::string target_name)
{
	for (std::vector<Transition>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if (it->GetSource()->GetName() == source_name && it->GetTarget()->GetName() == target_name) {
			it = transitions.erase(it);
		}
	}
}

ax::NodeEditor::EditorContext * ResourceAnimatorController::GetEditorContext()
{
	return ed_context;
}

std::string ResourceAnimatorController::GetName()
{
	return name;
}

Clip::Clip()
{
}

Clip::Clip(std::string name, uint resource_id, bool loop)
{
	this->name = name;
	this->resource_id = resource_id;
	this->loop = loop;
}

void Clip::SetName(std::string name)
{
	this->name = name;
}

void Clip::SetResource(uint resource_id)
{
	this->resource_id = resource_id;
}

void Clip::SetLoop(bool loop)
{
	this->loop = loop;
}

std::string Clip::GetName()
{
	return name;
}

uint Clip::GetResource()
{
	return resource_id;
}

bool Clip::GetLoop()
{
	return loop;
}

State::State()
{
}

State::State(std::string name, Clip * clip)
{
	this->name = name;
	this->clip = clip;
}

void State::SetName(std::string name)
{
	this->name = name;
}

void State::SetClip(Clip * clip)
{
	this->clip = clip;
}

std::string State::GetName()
{
	return std::string();
}

Clip * State::GetClip()
{
	return clip;
}

Transition::Transition()
{
}

Transition::Transition(State * source, State * target, bool trigger, unsigned blend)
{
	this->source = source;
	this->target = target;
	this->trigger = trigger;
	this->blend = blend;
}

void Transition::SetSource(State * source)
{
	this->source = source;
}

void Transition::SetTarget(State * target)
{
	this->target = target;
}

void Transition::SetTrigger(bool trigger)
{
	this->trigger = trigger;
}

void Transition::SetBlend(unsigned blend)
{
	this->blend = blend;
}

State * Transition::GetSource()
{
	return source;
}

State * Transition::GetTarget()
{
	return target;
}

bool Transition::GetTrigger()
{
	return trigger;
}

unsigned Transition::GetBlend()
{
	return blend;
}
