#include "Application.h"
#include "Globals.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "MetaFile.h"
#include "ResourceAnimatorController.h"
#include "ResourceAnimation.h"
#include "nlohmann\json.hpp"
#include <fstream>
#include <iomanip>

#include "AnimatorControllerImporter.h"



AnimatorControllerImporter::AnimatorControllerImporter()
{
}


AnimatorControllerImporter::~AnimatorControllerImporter()
{
}

bool AnimatorControllerImporter::Init()
{
	return false;
}

bool AnimatorControllerImporter::CleanUp()
{
	return true;
}

bool AnimatorControllerImporter::Import(const char * file, std::string & output_file, MetaFile *& meta)
{
	if (!meta) //If there was no meta, we create a new one for this resource and generate id.
	{
		meta = new MetaFile();
		meta->id = App->GenerateID();
	}
	
	std::ifstream i(file);
	nlohmann::json j_file = nlohmann::json::parse(i);

	name = j_file["name"].get<std::string>();

	nlohmann::json states_obj = j_file["states"];
	if (!states_obj.is_null())
	{
		for (nlohmann::json::iterator state = states_obj.begin(); state != states_obj.end(); ++state)
		{
			Tmp_State new_state;
			new_state.clip = state.value()["clip"];
			new_state.name = state.value()["name"].get<std::string>();
			new_state.speed = state.value()["speed"];

			states.push_back(new_state);
		}
	}

	nlohmann::json transitions_obj = j_file["transitions"];
	if (!transitions_obj.is_null())
	{
		for (nlohmann::json::iterator transition = transitions_obj.begin(); transition != transitions_obj.end(); ++transition)
		{
			Tmp_Transition new_transition;
			new_transition.source = transition.value()["source"].get<std::string>();
			new_transition.target = transition.value()["target"].get<std::string>();
			new_transition.trigger_num = transition.value()["trigger"];
			new_transition.blend = transition.value()["blend"];

			transitions.push_back(new_transition);
		}
	}

	output_file = LIBRARY_ANIM_CONTROLLER_FOLDER + std::to_string(meta->id) + ANIM_CONTROLLER_EXTENSION;
	Save(output_file, states,transitions,name);

	App->fsystem->GetFileModificationDate(file, meta->modification_date);
	meta->exported_file = output_file;
	meta->original_file = file;
	meta->file = std::string(file) + ".meta";
	meta->loaded = true;
	SaveMeta(meta);

	transitions.clear();
	states.clear();
	name.clear();

	return true;
}

bool AnimatorControllerImporter::Load(ResourceAnimatorController* resource)
{
	char* buffer;
	uint size = App->fsystem->Load(resource->GetExportedFile(), &buffer);
	char* cursor = buffer;

	//Load name size
	uint bytes = sizeof(uint);
	uint name_size;
	memcpy(&name_size, cursor, bytes);
	cursor += bytes;

	//Load name
	bytes = name_size;
	resource->name.resize(bytes);
	memcpy(&resource->name[0], cursor, bytes);
	cursor += bytes;

	//Load transitions and states nums
	bytes = sizeof(uint);
	uint num_states;
	memcpy(&num_states, cursor, bytes);
	cursor += bytes;
	uint num_transitions;
	memcpy(&num_transitions, cursor, bytes);
	cursor += bytes;

	for (int i = 0; i < num_states; i++)
	{
		//Load name size
		uint bytes = sizeof(uint);
		name_size;
		memcpy(&name_size, cursor, bytes);
		cursor += bytes;
		//Load name
		bytes = name_size;
		std::string tmp_name;
		tmp_name.resize(name_size);
		memcpy(&tmp_name[0], cursor, bytes);
		cursor += bytes;

		//Load clip id and speed
		bytes = sizeof(uint);
		uint clip_id;
		memcpy(&clip_id, cursor, bytes);
		cursor += bytes;
		bytes = sizeof(float);
		float speed;
		memcpy(&speed, cursor, bytes);
		cursor += bytes;

		resource->AddState(tmp_name, (ResourceAnimation*)App->resources->GetAndReference(clip_id), speed);
	}

	for (int i =0; i<num_transitions;++i)
	{
		//Load name size
		uint bytes = sizeof(uint);
		name_size;
		memcpy(&name_size, cursor, bytes);
		cursor += bytes;
		//Load name
		bytes = name_size;
		std::string tmp_source;
		tmp_source.resize(name_size);
		memcpy(&tmp_source[0], cursor, bytes);
		cursor += bytes;

		//Load name size
		bytes = sizeof(uint);
		name_size;
		memcpy(&name_size, cursor, bytes);
		cursor += bytes;
		//Load name
		bytes = name_size;
		std::string tmp_target;
		tmp_target.resize(name_size);
		memcpy(&tmp_target[0], cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint);
		uint tmp_trigger;
		memcpy(&tmp_trigger, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint);
		uint tmp_blend;
		memcpy(&tmp_blend, cursor, bytes);
		cursor += bytes;

		resource->AddTransition(resource->FindState(tmp_source), resource->FindState(tmp_target), tmp_blend, tmp_trigger);
	}

	LOG("Loaded Anim Controller %s", resource->GetExportedFile());

	RELEASE_ARRAY(buffer);
	return true;
}

bool AnimatorControllerImporter::Save(std::string file, ResourceAnimatorController * resource)
{
	name = resource->name;

	std::vector<State*> r_states = resource->GetStates();
	for (std::vector<State*>::iterator it = r_states.begin(); it != r_states.end(); ++it)
	{
		Tmp_State new_state;
		new_state.clip = (*it)->GetClip() ? (*it)->GetClip()->GetID() : 0;
		new_state.name = (*it)->GetName();
		new_state.speed = (*it)->GetSpeed();

		states.push_back(new_state);
	}

	std::vector<Transition*> r_transitions = resource->GetTransitions();
	for (std::vector<Transition*>::iterator it = r_transitions.begin(); it != r_transitions.end(); ++it)
	{
		Tmp_Transition new_transition;
		new_transition.source = (*it)->GetSource()->GetName();
		new_transition.target = (*it)->GetTarget()->GetName();
		new_transition.trigger_num = (*it)->GetTrigger();
		new_transition.blend = (*it)->GetBlend();

		transitions.push_back(new_transition);
	}

	std::string output_file = LIBRARY_ANIM_CONTROLLER_FOLDER + std::to_string(resource->meta->id) + ANIM_CONTROLLER_EXTENSION;
	Save(output_file, states, transitions, name);
	resource->SetFile(file);
	resource->SetExportedFile(output_file);

	App->fsystem->GetFileModificationDate(file.c_str(), resource->meta->modification_date);
	resource->meta->exported_file = output_file;
	resource->meta->file = std::string(file) + ".meta";
	resource->meta->loaded = true;

	SaveMeta(resource->meta);


	transitions.clear();
	states.clear();
	name.clear();

	return true;
}

bool AnimatorControllerImporter::Save(std::string file, std::vector<Tmp_State> states, std::vector<Tmp_Transition> transitions, std::string name)
{
	uint size = sizeof(uint) + name.size() + sizeof(uint) * 2;
	for (std::vector<Tmp_State>::iterator it = states.begin(); it != states.end(); ++it)
	{
		size += sizeof(uint) + (*it).name.size() + sizeof(uint) + sizeof(float);
	}
	for (std::vector<Tmp_Transition>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		size += sizeof(uint) + (*it).source.size() + sizeof(uint) + (*it).target.size() + sizeof(uint) * 2;
	}
	// Allocate
	char* data = new char[size];
	char* cursor = data;

	// Store name size and name
	uint bytes = sizeof(uint);
	uint name_size = name.size();
	memcpy(cursor, &name_size, bytes);
	cursor += bytes;
	bytes = name.size();
	memcpy(cursor, name.c_str(), bytes);
	cursor += bytes;

	//Store transitions and states nums
	bytes = sizeof(uint);
	uint num_states = states.size();
	memcpy(cursor, &num_states, bytes);
	cursor += bytes;
	uint num_transitions = transitions.size();
	memcpy(cursor, &num_transitions, bytes);
	cursor += bytes;

	for (std::vector<Tmp_State>::iterator it = states.begin(); it != states.end(); ++it)
	{
		// Store name size and name
		bytes = sizeof(uint);
		name_size = (*it).name.size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = (*it).name.size();
		memcpy(cursor, (*it).name.c_str(), bytes);
		cursor += bytes;

		//Store clip id and speed
		bytes = sizeof(uint);
		memcpy(cursor, &(*it).clip, bytes);
		cursor += bytes;
		bytes = sizeof(float);
		memcpy(cursor, &(*it).speed, bytes);
		cursor += bytes;
	}

	for (std::vector<Tmp_Transition>::iterator it = transitions.begin(); it != transitions.end(); ++it)
	{
		// Store name size and name
		bytes = sizeof(uint);
		name_size = (*it).source.size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = (*it).source.size();
		memcpy(cursor, (*it).source.c_str(), bytes);
		cursor += bytes;

		// Store name size and name
		bytes = sizeof(uint);
		name_size = (*it).target.size();
		memcpy(cursor, &name_size, bytes);
		cursor += bytes;
		bytes = (*it).target.size();
		memcpy(cursor, (*it).target.c_str(), bytes);
		cursor += bytes;

		bytes = sizeof(uint);
		memcpy(cursor, &(*it).trigger_num, bytes);
		cursor += bytes;

		bytes = sizeof(uint);
		memcpy(cursor, &(*it).blend, bytes);
		cursor += bytes;
	}

	uint ret = App->fsystem->Save(file.c_str(), data, size);
	RELEASE_ARRAY(data);

	return true;
}

bool AnimatorControllerImporter::SaveMeta(MetaFile * meta)
{
	nlohmann::json meta_file;

	meta_file = {
		{ "original_file", meta->original_file },
		{ "exported_file", meta->exported_file },
		{ "id", meta->id },
		{ "modification_date", meta->modification_date },
	};

	std::ofstream o(meta->file);
	o << std::setw(4) << meta_file << std::endl;

	return true;
}

bool AnimatorControllerImporter::LoadMeta(const char * file, MetaFile * meta)
{
	std::ifstream i(file);
	nlohmann::json json = nlohmann::json::parse(i);

	meta->file = file;
	meta->exported_file = json["exported_file"].get<std::string>();
	meta->original_file = json["original_file"].get<std::string>();
	meta->id = json["id"];
	meta->modification_date = json["modification_date"];

	meta->loaded = true;

	return meta;
}


