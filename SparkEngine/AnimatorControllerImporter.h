#ifndef _ANIMATOR_CONTROLLER_IMPORTER_H_
#define _ANIMATOR_CONTROLLER_IMPORTER_H_

#include "Importer.h"

class ResourceAnimatorController;
class MetaFile;

class AnimatorControllerImporter :
	public Importer
{
private:
	struct State {
		uint clip;
		std::string name;
		float speed;
	};
	struct Transition {
		std::string source;
		std::string target;
	};

public:
	AnimatorControllerImporter();
	~AnimatorControllerImporter();

	bool Init();
	bool CleanUp();

	bool Import(const char * file, std::string & output_file, MetaFile*& meta);
	bool Load(ResourceAnimatorController* resource);
	bool Save(std::string file, ResourceAnimatorController* resource);
	bool SaveMeta(MetaFile* meta);
	bool LoadMeta(const char* file, MetaFile* meta);

private:
	bool Save(std::string file, std::vector<State> states, std::vector<Transition> transitions, std::string name);

private:
	std::vector<State> states;
	std::vector<Transition> transitions;
	std::string name;
};


#endif // !_ANIMATOR_CONTROLLER_IMPORTER_H_


