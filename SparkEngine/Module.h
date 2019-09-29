#pragma once
#include "nlohmann\json.hpp"

class Module
{
private :
	bool enabled;

public:
	Module(bool start_enabled = true)
	{}

	virtual ~Module()
	{}

	virtual bool Init(nlohmann::json::iterator it) 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual bool Load(nlohmann::json::iterator it)
	{
		return true;
	}

	virtual bool Save(nlohmann::json &it)
	{
		return true;
	}

public:

	std::string name;
};