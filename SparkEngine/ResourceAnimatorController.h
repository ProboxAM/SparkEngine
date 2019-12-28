#ifndef _ANIMATOR_CONTROLLER_H_
#define _ANIMATOR_CONTROLLER_H_

#include "Resource.h"
#include "Globals.h"
#include "NodeEditor/Include/imgui_node_editor.h"

#include <vector>

class ResourceAnimation;

class State
{
private:
	std::string name;
	float speed = 1.0;
	ResourceAnimation* clip;

public:
	uint pin_in_id, pin_out_id, id;
	float time = 0;

public:
	State();
	State(std::string name, ResourceAnimation* clip);

public:
	void SetName(std::string name);
	void SetClip(ResourceAnimation* clip);

	std::string GetName();
	ResourceAnimation* GetClip();
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


class ResourceAnimatorController: public Resource
{
	State* current_state = nullptr;
	std::vector<State*> states;
	std::vector<Transition*> transitions;
	State* default_state = nullptr;

private:
	ax::NodeEditor::EditorContext* ed_context = nullptr;
	std::string name = "New Animator Controller";
	
public:
	ResourceAnimatorController(uint id);
	~ResourceAnimatorController();

public:

	//AnimationHandle
	void Play();
	void Play(std::string state_name);
	void Update();
	void Stop();


	//Transform
	bool GetTransform(std::string channel_name, float3 &position, Quat &rotation, float3 &scale);

	//States
	void AddState(std::string name, ResourceAnimation* clip);
	void RemoveState(std::string name);
	State* FindState(std::string name);
	State* FindState(uint id);
	State* FindStateFromPinId(uint pin_id);

	uint GetNumStates() const {return states.size(); }
	std::vector<State*> GetStates()  { return states; }

	//Transitions
	void AddTransition(State* source, State* target, bool trigger, uint blend);
	void RemoveTransition(std::string source_name, std::string target_name);
	std::vector<Transition*> GetTransitions() const { return transitions; }
	uint GetNumTransitions() const { return transitions.size(); }

	State* GetDefaultNode() const { return default_state; };

	ax::NodeEditor::EditorContext* GetEditorContext();

	std::string GetName();
};

#endif // !__ANIMATOR_CONTROLLER_H_
