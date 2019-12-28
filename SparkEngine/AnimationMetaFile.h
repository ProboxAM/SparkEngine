#pragma once
#include "MetaFile.h"
class AnimationMetaFile :
	public MetaFile
{
public:
	AnimationMetaFile();
	~AnimationMetaFile();

	std::string name;
	bool loops = false;
	uint start_tick = 0;
	uint end_tick = 0;
	uint max_tick = 0;
};