#include "Application.h"
#include "ModuleResources.h"
#include "ModuleTime.h"

#include "ResourceAnimation.h"

#include "ResourceAnimatorController.h"



ResourceAnimatorController::ResourceAnimatorController(uint id) : Resource(id, Resource::RESOURCE_TYPE::R_ANIMATOR)
{
	ed_context = ax::NodeEditor::CreateEditor();

	for (int i = 0; i < NUM_TRIGGERS; ++i) {
		triggers.push_back(false);
	}
}

ResourceAnimatorController::~ResourceAnimatorController()
{
	ax::NodeEditor::DestroyEditor(ed_context);
}


void ResourceAnimatorController::Update()
{
	if (current_state)
	{
		ResourceAnimation* animation = current_state->GetClip();

		if (animation && animation->GetDuration() > 0) {

			current_state->time += App->time->DeltaTime();
			
			LOG("current time: %f animation duration: %f", current_state->time, (float)animation->GetDuration());
			if (current_state->time >= animation->GetDuration()) {

				if (current_state->GetClip()->loops)
					current_state->time = 0;
				else
					current_state->time = animation->GetDuration();
			}

			CheckTriggers();
		}
	}
}

void ResourceAnimatorController::Stop()
{
	current_state = nullptr;
}

void ResourceAnimatorController::CheckTriggers()
{
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if ((*it)->GetTrigger() == true) {
			next_state = (*it)->GetTarget();
		}
	}
}

void ResourceAnimatorController::Play()
{
	current_state = default_state;
}

void ResourceAnimatorController::Play(std::string state_name)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it)
	{
		if ((*it)->GetName() == state_name)
			current_state = (*it);
	}
}

bool ResourceAnimatorController::GetTransform(std::string channel_name, float3 & position, Quat & rotation, float3 & scale)
{
	if (current_state)
	{
		ResourceAnimation* animation = current_state->GetClip();

		if (animation)
		{
			uint channel_index = animation->GetChannelIndex(channel_name);

			if (channel_index < animation->num_channels) {

				float3 next_position, next_scale;
				Quat next_rotation;
				float previous_key_time, next_key_time, t = 0;

				float time_in_ticks = animation->start_tick + (current_state->time * animation->ticks_per_second);

				if (animation->channels[channel_index].num_position_keys > 1)
				{
					for (int i = 0; i < animation->channels[channel_index].num_position_keys; i++)
					{
						if (time_in_ticks < animation->channels[channel_index].position_keys[i + 1].time) {
							position = animation->channels[channel_index].position_keys[i].value;
							next_position = animation->channels[channel_index].position_keys[i + 1].value;
							next_key_time = animation->channels[channel_index].position_keys[i + 1].time;
							t = (float)((double)time_in_ticks / next_key_time);
							LOG("CURRENT T IS %f", t);
							break;
						}
					}

					position = float3::Lerp(position, next_position, t);

				}else
					position = animation->channels[channel_index].position_keys[0].value;

				if (animation->channels[channel_index].num_rotation_keys > 1)
				{
					for (int i = 0; i < animation->channels[channel_index].num_rotation_keys; i++)
					{
						if (time_in_ticks < animation->channels[channel_index].rotation_keys[i + 1].time) {
							rotation = animation->channels[channel_index].rotation_keys[i].value;
							next_rotation = animation->channels[channel_index].rotation_keys[i + 1].value;
							next_key_time = animation->channels[channel_index].rotation_keys[i + 1].time;
							t = (float)((double)time_in_ticks / next_key_time);
							break;
						}
					}

					rotation = Quat::Slerp(rotation, next_rotation, t);


				}else
					rotation = animation->channels[channel_index].rotation_keys[0].value;



				for (int i = 0; i < animation->channels[channel_index].num_scale_keys; i++)
				{
					if (time_in_ticks < animation->channels[channel_index].scale_keys[i + 1].time) {
						scale = animation->channels[channel_index].scale_keys[i].value;
						next_scale = animation->channels[channel_index].scale_keys[i + 1].value;
						next_key_time = animation->channels[channel_index].scale_keys[i + 1].time;
						t = (float)((double)time_in_ticks / next_key_time);
						break;
					}
				}

				scale = float3::Lerp(scale, next_scale, t);

				return true;
			}else 
				return false;
		}else
			return false;
	}else
		return false;
}

void ResourceAnimatorController::AddState(std::string name, ResourceAnimation* clip)
{
	State* new_state = new State(name, clip);
	states.push_back(new_state);

	if (!default_state)
		default_state = new_state;
}

void ResourceAnimatorController::RemoveState(std::string name)
{
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if ((*it)->GetSource()->GetName() == name || (*it)->GetTarget()->GetName() == name) {
			delete (*it);
			it = transitions.erase(it);
			break;
		}
	}

	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->GetName() == name) {
			delete (*it);
			it = states.erase(it);
			break;
		}
	}
}

State* ResourceAnimatorController::FindState(std::string name)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->GetName() == name)
			return (*it);
	}
}

State * ResourceAnimatorController::FindState(uint id)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->id == id)
			return (*it);
	}
}

State * ResourceAnimatorController::FindStateFromPinId(uint pin_id)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->pin_in_id == pin_id || (*it)->pin_out_id == pin_id)
			return (*it);
	}
}

void ResourceAnimatorController::AddTransition(State * source, State * target, uint blend)
{
	Transition* new_transition = new Transition(source, target, blend);

	transitions.push_back(new_transition);
}

void ResourceAnimatorController::RemoveTransition(std::string source_name, std::string target_name)
{
	for (std::vector<Transition*>::iterator it = transitions.begin(); it != transitions.end(); ++it) {
		if ((*it)->GetSource()->GetName() == source_name && (*it)->GetTarget()->GetName() == target_name) {
			delete (*it);
			it = transitions.erase(it);
			break;
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

State::State()
{
}

State::State(std::string name, ResourceAnimation * clip)
{
	this->name = name;
	this->clip = clip;
}

void State::SetName(std::string name)
{
	this->name = name;
}

void State::SetClip(ResourceAnimation * clip)
{
	clip->AddReference();
	this->clip = clip;
}

std::string State::GetName()
{
	return name;
}

ResourceAnimation * State::GetClip()
{
	return clip;
}

Transition::Transition()
{
}

Transition::Transition(State * source, State * target, unsigned blend)
{
	this->source = source;
	this->target = target;
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

void Transition::SetTrigger(uint trigger)
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

uint Transition::GetTrigger()
{
	return trigger;
}

unsigned Transition::GetBlend()
{
	return blend;
}
