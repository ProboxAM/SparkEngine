#include "Application.h"
#include "ModuleResources.h"
#include "ModuleTime.h"

#include "ResourceAnimation.h"

#include "ResourceAnimatorController.h"



ResourceAnimatorController::ResourceAnimatorController(uint id) : Resource(id, Resource::RESOURCE_TYPE::R_ANIMATOR)
{
	ed_context = ax::NodeEditor::CreateEditor();
}

ResourceAnimatorController::~ResourceAnimatorController()
{
	ax::NodeEditor::DestroyEditor(ed_context);
}

void ResourceAnimatorController::PlayClip(std::string name, uint resource_id, bool loop)
{
	Instance* new_instance = new Instance();

	new_instance->clip = FindClip(name);
	new_instance->loop = loop;
	new_instance->next = current_playing;

	current_playing = new_instance;
}

void ResourceAnimatorController::Update()
{
	if (current_playing)
	{
		ResourceAnimation* animation = (ResourceAnimation*)App->resources->Get(current_playing->clip->GetResource());

		if (animation && animation->GetDuration() > 0) {

			current_playing->time += App->time->DeltaTime();
			
			LOG("current time: %f animation duration: %f", current_playing->time, (float)animation->GetDuration());
			if (current_playing->time >= animation->GetDuration()) {

				if (current_playing->loop)
					current_playing->time = 0;
				else
					current_playing->time = animation->GetDuration();
			}
		}
	}
}

void ResourceAnimatorController::Stop()
{
	if (current_playing) {
		current_playing = nullptr;
	}
}

bool ResourceAnimatorController::GetTransform(std::string channel_name, float3 & position, Quat & rotation, float3 & scale)
{
	if (current_playing)
	{
		ResourceAnimation* animation = (ResourceAnimation*)App->resources->Get(current_playing->clip->GetResource());

		if (animation)
		{
			uint channel_index = animation->GetChannelIndex(channel_name);

			if (channel_index < animation->num_channels) {

				float3 next_position, next_scale;
				Quat next_rotation;
				float previous_key_time, next_key_time, t = 0;

				float time_in_ticks = current_playing->time * animation->ticks_per_second;

				if (animation->channels[channel_index].num_position_keys > 1)
				{
					for (int i = 0; i < animation->channels[channel_index].num_position_keys; i++)
					{
						if (time_in_ticks < animation->channels[channel_index].position_keys[i + 1].time) {
							position = animation->channels[channel_index].position_keys[i].value;
							next_position = animation->channels[channel_index].position_keys[i + 1].value;
							next_key_time = animation->channels[channel_index].position_keys[i + 1].time;
							t = (float)((double)time_in_ticks / next_key_time);
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

void ResourceAnimatorController::AddClip(std::string name, uint id, bool loop)
{
	Clip* new_clip = new Clip(name, id, loop);
	clips.push_back(new_clip);
}

void ResourceAnimatorController::RemoveClip(std::string name)
{
	for (std::vector<State*>::iterator it = states.begin(); it != states.end(); ++it) {
		if ((*it)->GetClip()->GetName() == name) {
			it = states.erase(it);
		}
	}

	for (std::vector<Clip*>::iterator it = clips.begin(); it != clips.end(); ++it) {
		if ((*it)->GetName() == name) {
			it = clips.erase(it);
		}
	}
}

Clip* ResourceAnimatorController::FindClip(std::string name)
{
	for (std::vector<Clip*>::iterator it = clips.begin(); it != clips.end(); ++it) {
		if ((*it)->GetName() == name)
			return (*it);
	}
}

void ResourceAnimatorController::AddState(std::string name, Clip * clip)
{
	State* new_state = new State(name, clip);
	states.push_back(new_state);
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

void ResourceAnimatorController::AddTransition(State * source, State * target, bool trigger, uint blend)
{
	Transition* new_transition = new Transition(source, target, trigger, blend);

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
	return name;
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
