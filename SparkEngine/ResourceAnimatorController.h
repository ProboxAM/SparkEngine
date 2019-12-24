#ifndef _ANIMATOR_CONTROLLER_H_
#define _ANIMATOR_CONTROLLER_H_

#include "Resource.h"
#include "Globals.h"
#include "NodeEditor/Include/imgui_node_editor.h"

#include <vector>

class Clip
{
private:
	std::string      name;
	uint             resource_id = 0;
	bool             loop = false;

public:
	Clip();
	Clip(std::string name, uint resource_id, bool loop);

public:
	void SetName(std::string name);
	void SetResource(uint resource_id);
	void SetLoop(bool loop);

	std::string GetName();
	uint GetResource();
	bool GetLoop();
};

class State
{
private:
	std::string name;
	Clip* clip;

public:
	State();
	State(std::string name, Clip* clip);

public:
	void SetName(std::string name);
	void SetClip(Clip* clip);

	std::string GetName();
	Clip* GetClip();
};

class Transition
{
private:
	State* source;
	State* target;
	bool trigger;
	unsigned blend = 200;

public:
	Transition();
	Transition(State* source, State* target, bool trigger, unsigned blend);

public:
	void SetSource(State* source);
	void SetTarget(State* target);
	void SetTrigger(bool trigger);
	void SetBlend(unsigned blend);

	State* GetSource();
	State* GetTarget();
	bool GetTrigger();
	unsigned GetBlend();
};


class ResourceAnimatorController:
	public Resource
{

	struct Instance
	{
		Clip clip;
		float time = 0;
		bool     loop = true;
		float    speed = 1.0;

		Instance* next = nullptr;
		uint fade_duration = 0;
		uint fade_time = 0;
	};

	Instance* current_playing = nullptr;


	std::vector<Clip> clips;
	std::vector<State> states;
	std::vector<Transition> transitions;
	uint entry_node = 0;

private:
	ax::NodeEditor::EditorContext* ed_context = nullptr;
	std::string name = "New Animator Controller";
	
public:
	ResourceAnimatorController(uint id);
	~ResourceAnimatorController();

public:

	//AnimationHandle
	void PlayClip(std::string name, uint resource_id, bool loop);
	void Update();
	void Stop();

	//Transform
	bool GetTransform(std::string channel_name, float3 &position, Quat &rotation, float3 &scale);

	//Clips
	void AddClip(std::string name, uint id, bool loop);
	void RemoveClip(std::string name);
	Clip FindClip(std::string name);

	uint GetNumClips() const { return clips.size(); }
	std::vector<Clip> GetClips() const { return clips; }

	//States
	void AddState(std::string name, Clip* clip);
	void RemoveState(std::string name);
	State FindState(std::string name);

	uint GetNumStates() const {return states.size(); }
	std::vector<State> GetStates() const { return states; }

	//Transitions
	void AddTransition(State* source, State* target, bool trigger, uint blend);
	void RemoveTransition(std::string source_name, std::string target_name);
	std::vector<Transition> GetTransitions() const { return transitions; }

	uint GetDefaultNode() const { return entry_node; };

	ax::NodeEditor::EditorContext* GetEditorContext();

	std::string GetName();
};

#endif // !__ANIMATOR_CONTROLLER_H_
